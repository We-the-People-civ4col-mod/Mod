const std = @import("std");
const assert = std.debug.assert;

pub fn main() !void {
    const arena = std.heap.page_allocator;

    var args = try std.process.argsWithAllocator(arena);

    assert(args.skip()); // executable
    const input_path = args.next() orelse @panic("Missing input path");
    const output_path = args.next() orelse @panic("Missing output path");

    const dll = try std.fs.cwd().readFileAlloc(arena, input_path, std.math.maxInt(usize));

    const coff = try std.coff.Coff.init(dll, false);
    const coff_header: *align(1) std.coff.CoffHeader = @ptrCast(dll[coff.coff_header_offset..][0..@sizeOf(std.coff.CoffHeader)]);
    assert(coff_header.machine == .I386);
    assert(coff_header.flags.@"32BIT_MACHINE" == 1);
    assert(coff_header.flags.DLL == 1);
    assert(coff_header.flags.LARGE_ADDRESS_AWARE == 0);
    coff_header.flags.LARGE_ADDRESS_AWARE = 1;

    try std.fs.cwd().writeFile(output_path, dll);
}
