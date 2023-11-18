const std = @import("std");
const assert = std.debug.assert;

pub fn main() !void {
    var gpa_instance = std.heap.GeneralPurposeAllocator(.{}){};
    defer assert(gpa_instance.deinit() == .ok);
    const gpa = gpa_instance.allocator();

    const writer = std.io.getStdOut().writer();

    var args = try std.process.argsAlloc(gpa);
    defer std.process.argsFree(gpa, args);
    try writer.writeAll("[\n");
    for (args[1..], 1..) |arg, i| {
        const comp_db = try std.fs.cwd().readFileAlloc(gpa, arg, std.math.maxInt(usize));
        defer gpa.free(comp_db);
        if (i == args.len - 1) {
            try writer.writeAll(comp_db[0 .. comp_db.len - 2]);
            try writer.writeByte('\n');
        } else {
            try writer.writeAll(comp_db);
        }
    }
    try writer.writeAll("]\n");
}
