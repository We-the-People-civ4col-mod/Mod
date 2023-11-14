const std = @import("std");
const assert = std.debug.assert;

pub fn main() !void {
    var arena_impl = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena_impl.deinit();
    const arena = arena_impl.allocator();

    var args_iter = try std.process.argsWithAllocator(arena);

    assert(args_iter.skip()); // argv[0] is the executable

    const git_rev = args_iter.next() orelse @panic("Missing git revision parameter");
    assert(git_rev.len > 0);

    const output_path = args_iter.next() orelse @panic("Missing output parameter");
    assert(output_path.len > 0);

    var output = try std.fs.cwd().createFile(output_path, .{});
    defer output.close();

    try output.writer().print(
        \\#ifndef AUTO_GIT_VERSION_H
        \\#define AUTO_GIT_VERSION_H
        \\#pragma once
        \\char const* szGitVersion = "{s}";
        \\#endif
    , .{git_rev});
}
