const std = @import("std");
const assert = std.debug.assert;

pub fn main() !void {
    var arena_impl = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    const arena = arena_impl.allocator();
    defer arena_impl.deinit();

    var args = try std.process.argsWithAllocator(arena);

    assert(args.skip()); // executable

    const mode = args.next() orelse @panic("Missing mode");
    if (std.mem.eql(u8, mode, "dump_exe")) {
        const vanilla_exe = args.next() orelse @panic("Missing vanilla EXE path");
        const vanilla_txt = args.next() orelse @panic("Missing vanilla TXT path");

        const vanilla = try extractImports(arena, vanilla_exe);
        try dumpImports(vanilla, vanilla_txt);
    } else if (std.mem.eql(u8, mode, "verify_dll")) {
        const vanilla_txt = args.next() orelse @panic("Missing vanilla TXT path");
        const modded_dll = args.next() orelse @panic("Missing modded DLL path");

        _ = std.fs.cwd().openFile(vanilla_txt, .{}) catch |err| switch (err) {
            error.FileNotFound => @panic("Missing vanilla EXE dump. Generate a new one using `zig build dump_exe -- <path to EXE>."),
            else => {},
        };

        const vanilla_imports = try parseImports(arena, vanilla_txt);
        const modded_exports = try extractExports(arena, modded_dll);

        var missing: usize = 0;
        for (vanilla_imports.keys()) |name| {
            if (!modded_exports.contains(name)) {
                std.log.err("missing: {s}", .{name});
                missing += 1;
            }
        }

        var extra: usize = 0;
        for (modded_exports.keys()) |name| {
            if (!vanilla_imports.contains(name)) {
                std.log.warn("extra: {s}", .{name});
                extra += 1;
            }
        }

        if (extra > 0) {
            std.log.warn("extra count: {d}", .{extra});
        }
        if (missing > 0) {
            std.log.err("missing count: {d}", .{missing});
            return error.MissingExports;
        }
    } else @panic("Unknown mode");
}

pub const ImageExportDirectory = extern struct {
    characteristics: u32,
    time_date_stamp: u32,
    major_version: u16,
    minor_version: u16,
    name: u32,
    base: u32,
    number_of_functions: u32,
    number_of_names: u32,
    address_of_functions: u32,
    address_of_names: u32,
    address_of_name_ordinals: u32,
};

pub const ImageDelayLoadDescriptor = extern struct {
    attributes: u32,
    name: u32,
    module_handle: u32,
    delay_import_address_table: u32,
    delay_import_name_table: u32,
    bound_delay_import_table: u32,
    unload_delay_import_table: u32,
    time_stamp: u32,
};

fn extractImports(arena: std.mem.Allocator, exe_path: []const u8) !std.StringArrayHashMap(void) {
    const exe = try std.fs.cwd().readFileAlloc(arena, exe_path, std.math.maxInt(usize));

    var coff = try std.coff.Coff.init(exe, false);
    assert(coff.is_image);

    const data_dirs = coff.getDataDirectories();
    const delay_dir = data_dirs[@intFromEnum(std.coff.DirectoryEntry.DELAY_IMPORT)];
    std.log.debug("delay_dir={any}", .{delay_dir});
    assert(delay_dir.size != 0);
    assert(delay_dir.virtual_address != 0);

    var stream = std.io.fixedBufferStream(coff.data);
    const reader = stream.reader();

    try seekToRva(&stream, coff, delay_dir.virtual_address);
    const delay_load_descriptor = try reader.readStruct(ImageDelayLoadDescriptor);
    std.log.debug("delay_load_descriptor={any}", .{delay_load_descriptor});

    try seekToRva(&stream, coff, delay_load_descriptor.name);
    const dll_name = try reader.readUntilDelimiterAlloc(arena, 0, std.math.maxInt(usize));
    std.log.debug("dll_name={s}", .{dll_name});
    assert(std.mem.eql(u8, dll_name, "CvGameCoreDLL.dll"));

    var imports = std.StringArrayHashMap(void).init(arena);
    errdefer imports.deinit();

    var i: u32 = 0;
    while (true) : (i += 1) {
        try seekToRva(&stream, coff, delay_load_descriptor.delay_import_name_table + i * 4);
        const raw = try reader.readInt(u32, .little);
        if (raw == 0) break;
        const entry = std.coff.ImportLookupEntry32.getImportByName(raw) orelse continue;
        std.log.debug("entry={any}", .{entry});

        try seekToRva(&stream, coff, entry.name_table_rva + 2); // 2 byte "hint"
        const name = try reader.readUntilDelimiterAlloc(arena, 0, std.math.maxInt(usize));
        std.log.debug("name={s}", .{name});

        try imports.putNoClobber(name, {});
    }
    std.log.debug("numer_of_imports={d}", .{i});

    return imports;
}

fn extractExports(arena: std.mem.Allocator, dll_path: []const u8) !std.StringArrayHashMap(void) {
    const dll = try std.fs.cwd().readFileAlloc(arena, dll_path, std.math.maxInt(usize));

    var coff = try std.coff.Coff.init(dll, false);
    assert(coff.is_image);

    const data_dirs = coff.getDataDirectories();
    const export_dir = data_dirs[@intFromEnum(std.coff.DirectoryEntry.EXPORT)];
    std.log.debug("export_dir={any}", .{export_dir});
    assert(export_dir.size != 0);
    assert(export_dir.virtual_address != 0);

    var stream = std.io.fixedBufferStream(coff.data);
    const reader = stream.reader();

    try seekToRva(&stream, coff, export_dir.virtual_address);
    const export_directory = try reader.readStruct(ImageExportDirectory);
    std.log.debug("export_directory={any}", .{export_directory});
    assert(export_directory.major_version == 0);
    assert(export_directory.minor_version == 0);
    assert(export_directory.base == 1);

    var exports = std.StringArrayHashMap(void).init(arena);
    errdefer exports.deinit();

    try seekToRva(&stream, coff, export_directory.name);
    const dll_name = try reader.readUntilDelimiterAlloc(arena, 0, export_dir.size);
    std.log.debug("dll_name={s}", .{dll_name});
    assert(std.mem.eql(u8, dll_name, "CvGameCoreDLL.dll"));

    std.log.debug("number_of_functions={x}", .{export_directory.number_of_functions});
    assert(export_directory.number_of_functions == export_directory.number_of_names); // we only support names, not ordinals

    var i: u32 = 0;
    while (i < export_directory.number_of_names) : (i += 1) {
        try seekToRva(&stream, coff, export_directory.address_of_names + i * 4);
        const name_rva = try reader.readInt(u32, .little);
        std.log.debug("name_rva={x}", .{name_rva});

        try seekToRva(&stream, coff, name_rva);
        const name = try reader.readUntilDelimiterAlloc(arena, 0, std.math.maxInt(usize));
        std.log.debug("{s}", .{name});

        try exports.putNoClobber(name, {});
    }

    return exports;
}

fn seekToRva(stream: *std.io.FixedBufferStream([]const u8), coff: std.coff.Coff, rva: u32) !void {
    const section = for (coff.getSectionHeaders()) |*sect| {
        if (rva >= sect.virtual_address and rva < sect.virtual_address + sect.virtual_size) {
            break sect;
        }
    } else {
        unreachable;
    };
    const section_offset = rva - section.virtual_address;
    const file_offset = section.pointer_to_raw_data + section_offset;
    try stream.seekTo(file_offset);
}

fn dumpImports(imports: std.StringArrayHashMap(void), txt_path: []const u8) !void {
    var txt = try std.fs.cwd().createFile(txt_path, .{});
    defer txt.close();
    var buffered_writer = std.io.bufferedWriter(txt.writer());
    const writer = buffered_writer.writer();
    for (imports.keys()) |name| {
        try writer.writeAll(name);
        try writer.writeByte('\n');
    }
    try buffered_writer.flush();
}

fn parseImports(arena: std.mem.Allocator, txt_path: []const u8) !std.StringArrayHashMap(void) {
    const txt = try std.fs.cwd().readFileAlloc(arena, txt_path, std.math.maxInt(usize));
    var imports = std.StringArrayHashMap(void).init(arena);
    var line_iter = std.mem.tokenizeScalar(u8, txt, '\n');
    while (line_iter.next()) |line| {
        try imports.putNoClobber(line, {});
    }
    return imports;
}
