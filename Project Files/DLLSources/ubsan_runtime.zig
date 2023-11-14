const std = @import("std");
const assert = std.debug.assert;

const ErrorType = enum {
    generic_ub,
    null_pointer_use,
    null_pointer_use_with_nullability,
    nullptr_with_offset,
    nullptr_with_nonzero_offset,
    nullptr_after_nonzero_offset,
    pointer_overflow,
    misaligned_pointer_use,
    alignment_assumption,
    insufficient_object_size,
    signed_integer_overflow,
    unsigned_integer_overflow,
    integer_divide_by_zero,
    float_divide_by_zero,
    invalid_builtin,
    invalid_objc_cast,
    implicit_unsigned_integer_truncation,
    implicit_signed_integer_truncation,
    implicit_integer_sign_change,
    implicit_signed_integer_truncation_or_sign_change,
    invalid_shift_base,
    invalid_shift_exponent,
    out_of_bounds_index,
    unreachable_call,
    missing_return,
    non_positive_vla_index,
    float_cast_overflow,
    invalid_bool_load,
    invalid_enum_load,
    function_type_mismatch,
    invalid_null_return,
    invalid_null_return_with_nullability,
    invalid_null_argument,
    invalid_null_argument_with_nullability,
    dynamic_type_mismatch,
    cfi_base_type,
};

const ValueHandle = usize;

const SourceLocation = extern struct {
    file: [*:0]const u8,
    line: u32,
    column: u32,

    pub fn format(location: SourceLocation, comptime _: []const u8, _: std.fmt.FormatOptions, writer: anytype) !void {
        try writer.print("{s}:{d}:{d}", .{ location.file, location.line, location.column });
    }

    fn formatFileName(location: SourceLocation, comptime _: []const u8, _: std.fmt.FormatOptions, writer: anytype) !void {
        const file = std.mem.span(location.file);
        const file_name = blk: {
            const slash_index = std.mem.lastIndexOfAny(u8, file, "/\\") orelse break :blk file;
            break :blk file[slash_index + 1 .. :0];
        };
        try writer.print("{s}:{d}:{d}", .{ file_name, location.line, location.column });
    }

    pub fn fmtFileName(location: SourceLocation) std.fmt.Formatter(formatFileName) {
        return .{ .data = location };
    }
};

const TypeDescriptor = opaque {
    const Layout = extern struct {
        kind: Kind,
        info: u16,
        name_vla_start: [0:0]u8,
    };

    const Kind = enum(u16) {
        integer = 0x0000,
        float = 0x0001,
        unknown = 0xffff,
    };

    fn kind(type_descriptor: *const TypeDescriptor) Kind {
        const layout: *const Layout = @ptrCast(@alignCast(type_descriptor));
        return layout.kind;
    }

    fn info(type_descriptor: *const TypeDescriptor) u16 {
        const layout: *const Layout = @ptrCast(@alignCast(type_descriptor));
        return layout.info;
    }

    fn name(type_descriptor: *const TypeDescriptor) [:0]const u8 {
        const layout: *const Layout = @ptrCast(@alignCast(type_descriptor));
        const ptr: [*:0]const u8 = &layout.name_vla_start;
        return std.mem.span(ptr);
    }

    const Sign = enum(u1) { unsigned = 0, signed = 1 };
    fn intSign(type_descriptor: *const TypeDescriptor) Sign {
        assert(type_descriptor.kind() == .integer);
        return @enumFromInt(type_descriptor.info() & 1);
    }

    fn bitWidth(type_descriptor: *const TypeDescriptor) usize {
        return switch (type_descriptor.kind()) {
            .integer => blk: {
                const log2_bit_width: std.math.Log2Int(usize) = @intCast(type_descriptor.info() >> 1);
                break :blk @shlExact(@as(usize, 1), log2_bit_width);
            },
            .float => type_descriptor.info(),
            else => unreachable,
        };
    }

    pub const format = @compileError("Use `TypeDescriptor.fmtQuoted` or `TypeDescriptor.fmtUnquoted` instead!");

    fn formatQuoted(type_descriptor: *const TypeDescriptor, comptime _: []const u8, _: std.fmt.FormatOptions, writer: anytype) !void {
        try writer.writeAll(type_descriptor.name());
    }

    fn formatUnquoted(type_descriptor: *const TypeDescriptor, comptime _: []const u8, _: std.fmt.FormatOptions, writer: anytype) !void {
        try writer.writeAll(type_descriptor.name()[1 .. type_descriptor.name().len - 1]);
    }

    pub fn fmtQuoted(type_descriptor: *const TypeDescriptor) std.fmt.Formatter(formatQuoted) {
        return .{ .data = type_descriptor };
    }

    pub fn fmtUnquoted(type_descriptor: *const TypeDescriptor) std.fmt.Formatter(formatUnquoted) {
        return .{ .data = type_descriptor };
    }
};

/// TODO: Improve `Value` printing
const Value = struct {
    type: *const TypeDescriptor,
    value: ValueHandle,

    fn init(@"type": *const TypeDescriptor, value: ValueHandle) Value {
        return .{
            .type = @"type",
            .value = value,
        };
    }

    const inline_bits = @sizeOf(ValueHandle) * 8;

    fn isInline(value: Value) bool {
        return value.type.bitWidth() <= inline_bits;
    }

    fn isNegative(value: Value) bool {
        if (value.type.intSign() != .signed) return false;
        return value.int() < 0;
    }

    fn int(value: Value) isize {
        assert(value.type.intSign() == .signed);
        assert(value.type.bitWidth() <= @bitSizeOf(isize)); // we don't support i128/u128
        if (value.isInline()) {
            const extra_bits: std.math.Log2Int(isize) = @intCast(@bitSizeOf(isize) - value.type.bitWidth());
            const signed_shifted: isize = @bitCast(@shlExact(value.value, extra_bits));
            return @shrExact(signed_shifted, extra_bits);
        }
        assert(value.type.bitWidth() == @bitSizeOf(isize));
        const ptr: *const isize = @ptrFromInt(value.value);
        return ptr.*;
    }

    fn uint(value: Value) usize {
        assert(value.type.intSign() == .unsigned);
        assert(value.type.bitWidth() <= @bitSizeOf(usize)); // we don't support i128/u128
        if (value.isInline()) {
            return value.value;
        }
        assert(value.type.bitWidth() == @bitSizeOf(usize));
        const ptr: *const usize = @ptrFromInt(value.value);
        return ptr.*;
    }

    fn positiveInt(value: Value) usize {
        if (value.type.intSign() == .unsigned) {
            return value.uint();
        }
        const positive_int = value.int();
        assert(positive_int >= 0);
        return @intCast(positive_int);
    }

    pub fn format(value: *const Value, comptime _: []const u8, _: std.fmt.FormatOptions, writer: anytype) !void {
        try writer.print("({})0x{x}", .{ value.type.fmtUnquoted(), value.value });
    }
};

const TypeMismatchData = extern struct {
    location: SourceLocation,
    type: *const TypeDescriptor,
    log_alignment: u8,
    type_check_kind: TypeCheckKind,

    pub const TypeCheckKind = enum(u8) {
        load,
        store,
        reference_binding,
        member_access,
        member_call,
        constructor_call,
        downcast_pointer,
        downcast_reference,
        upcast,
        upcast_to_virtual_base,
        nonnull_assign,
        dynamic_operation,

        pub fn format(type_check_kind: TypeCheckKind, comptime _: []const u8, _: std.fmt.FormatOptions, writer: anytype) !void {
            try writer.writeAll(switch (type_check_kind) {
                .load => "load of",
                .store => "store to",
                .reference_binding => "reference binding to",
                .member_access => "member access within",
                .member_call => "member call on",
                .constructor_call => "constructor call on",
                .downcast_pointer, .downcast_reference => "downcast of",
                .upcast => "upcast of",
                .upcast_to_virtual_base => "cast to virtual base of",
                .nonnull_assign => "_Nonnull binding to",
                .dynamic_operation => "dynamic operation on",
            });
        }
    };
};
export fn __ubsan_handle_type_mismatch_v1(data: *const TypeMismatchData, pointer: ValueHandle) callconv(.C) void {
    const alignment = @as(usize, 1) << @intCast(data.log_alignment);
    const error_type: ErrorType = blk: {
        if (pointer == 0) {
            if (data.type_check_kind == .nonnull_assign) {
                break :blk .null_pointer_use_with_nullability;
            }
            break :blk .null_pointer_use;
        }
        if (pointer & (alignment - 1) != 0) {
            break :blk .misaligned_pointer_use;
        }
        break :blk .insufficient_object_size;
    };
    var buffer: [4096]u8 = undefined;
    const message = switch (error_type) {
        .null_pointer_use_with_nullability, .null_pointer_use => std.fmt.bufPrintZ(&buffer, "{} null pointer of type {}", .{ data.type_check_kind, data.type.fmtQuoted() }),
        .misaligned_pointer_use => std.fmt.bufPrintZ(&buffer, "{} misaligned address 0x{x} for type {}, which requires {d} byte alignment", .{ data.type_check_kind, pointer, data.type.fmtQuoted(), alignment }),
        .insufficient_object_size => std.fmt.bufPrintZ(&buffer, "{} address 0x{x} with insufficient space for an object of type {}", .{ data.type_check_kind, pointer, data.type.fmtQuoted() }),
        else => unreachable,
    } catch @trap();
    report(data.location, error_type, message);
}

/// TODO: Implement `__ubsan_handle_function_type_mismatch`
export fn __ubsan_handle_function_type_mismatch(data: *const anyopaque, value: ValueHandle) callconv(.C) void {
    _ = value;
    const location: *const SourceLocation = @ptrCast(@alignCast(data));
    report(location.*, .function_type_mismatch, "__ubsan_handle_function_type_mismatch");
}

const InvalidValueData = extern struct {
    location: SourceLocation,
    type: *const TypeDescriptor,
};
export fn __ubsan_handle_load_invalid_value(data: *const InvalidValueData, value: ValueHandle) callconv(.C) void {
    const is_bool = std.mem.startsWith(u8, data.type.name(), "'bool'") or std.mem.startsWith(u8, data.type.name(), "'BOOL'");
    const error_type: ErrorType = if (is_bool) .invalid_bool_load else .invalid_enum_load;
    var buffer: [4096]u8 = undefined;
    const message = std.fmt.bufPrintZ(&buffer, "load of value {}, which is not a valid value for type {}", .{ Value.init(data.type, value), data.type.fmtQuoted() }) catch @trap();
    report(data.location, error_type, message);
}

const PointerOverflowData = extern struct {
    location: SourceLocation,
    type: *const TypeDescriptor,
};
export fn __ubsan_handle_pointer_overflow(data: *const PointerOverflowData, base: ValueHandle, result: ValueHandle) callconv(.C) void {
    const error_type: ErrorType = blk: {
        if (base == 0 and result == 0) break :blk .nullptr_with_offset;
        if (base == 0 and result != 0) break :blk .nullptr_with_nonzero_offset;
        if (base != 0 and result == 0) break :blk .nullptr_after_nonzero_offset;
        break :blk .pointer_overflow;
    };
    var buffer: [4096]u8 = undefined;
    const message = switch (error_type) {
        .nullptr_with_offset => std.fmt.bufPrintZ(&buffer, "applying zero offset to null pointer of type {}", .{data.type.fmtQuoted()}),
        .nullptr_with_nonzero_offset => std.fmt.bufPrintZ(&buffer, "applying non-zero offset {d} to null pointer of type {}", .{ base, data.type.fmtQuoted() }),
        .nullptr_after_nonzero_offset => std.fmt.bufPrintZ(&buffer, "applying non-zero offset {d} to non-null pointer of type {} produced null pointer", .{ base, data.type.fmtQuoted() }),
        .pointer_overflow => blk: {
            if ((@as(isize, @bitCast(base)) >= 0) == (@as(isize, @bitCast(result)) >= 0)) {
                if (base > result) {
                    break :blk std.fmt.bufPrintZ(&buffer, "addition of unsigned offset to 0x{x} overflowed to 0x{x}", .{ base, result });
                }
                break :blk std.fmt.bufPrintZ(&buffer, "subtraction of unsigned offset from 0x{x} overflowed to 0x{x}", .{ base, result });
            }
            break :blk std.fmt.bufPrintZ(&buffer, "pointer index expression with 0x{x} overflowed to 0x{x}", .{ base, result });
        },
        else => unreachable,
    } catch @trap();
    report(data.location, error_type, message);
}

const OverflowData = extern struct {
    location: SourceLocation,
    type: *const TypeDescriptor,
};
inline fn handleIntegerOverflow(comptime operator: []const u8, data: *const OverflowData, lhs: ValueHandle, rhs: ValueHandle) void {
    const error_type: ErrorType = switch (data.type.intSign()) {
        .signed => .signed_integer_overflow,
        .unsigned => .unsigned_integer_overflow,
    };
    // if (data.type.intSign() == .unsigned) return;
    var buffer: [4096]u8 = undefined;
    const message = std.fmt.bufPrintZ(
        &buffer,
        "{s} integer overflow: {} " ++ operator ++ " {} cannot be represented in type {}",
        .{ @tagName(data.type.intSign()), Value.init(data.type, lhs), Value.init(data.type, rhs), data.type.fmtQuoted() },
    ) catch @trap();
    report(data.location, error_type, message);
}
export fn __ubsan_handle_add_overflow(data: *const OverflowData, lhs: ValueHandle, rhs: ValueHandle) callconv(.C) void {
    handleIntegerOverflow("+", data, lhs, rhs);
}
export fn __ubsan_handle_sub_overflow(data: *const OverflowData, lhs: ValueHandle, rhs: ValueHandle) callconv(.C) void {
    handleIntegerOverflow("-", data, lhs, rhs);
}
export fn __ubsan_handle_mul_overflow(data: *const OverflowData, lhs: ValueHandle, rhs: ValueHandle) callconv(.C) void {
    handleIntegerOverflow("*", data, lhs, rhs);
}

export fn __ubsan_handle_divrem_overflow(data: *const OverflowData, lhs: ValueHandle, rhs: ValueHandle) callconv(.C) void {
    const lhs_value = Value.init(data.type, lhs);
    const rhs_value = Value.init(data.type, rhs);
    const error_type: ErrorType = blk: {
        if (rhs_value.type.kind() == .integer and rhs_value.type.intSign() == .signed and rhs_value.int() == -1)
            break :blk .signed_integer_overflow;
        if (data.type.kind() == .integer)
            break :blk .integer_divide_by_zero;
        assert(data.type.kind() == .float);
        break :blk .float_divide_by_zero;
    };
    var buffer: [4096]u8 = undefined;
    const message = switch (error_type) {
        .signed_integer_overflow => std.fmt.bufPrintZ(&buffer, "division of {} by -1 cannot be represented in type {}", .{ lhs_value, data.type.fmtQuoted() }),
        .integer_divide_by_zero, .float_divide_by_zero => std.fmt.bufPrintZ(&buffer, "division by zero", .{}),
        else => unreachable,
    } catch @trap();
    report(data.location, error_type, message);
}

export fn __ubsan_handle_negate_overflow(data: *const OverflowData, old_value: ValueHandle) callconv(.C) void {
    const error_type: ErrorType = switch (data.type.intSign()) {
        .signed => .signed_integer_overflow,
        .unsigned => .unsigned_integer_overflow,
    };
    // if (data.type.intSign() == .unsigned) return;
    var buffer: [4096]u8 = undefined;
    const message = switch (error_type) {
        .signed_integer_overflow => std.fmt.bufPrintZ(
            &buffer,
            "negation of {} cannot be represented in type {}; cast to an unsigned type to negate this value to itself",
            .{ Value.init(data.type, old_value), data.type.fmtQuoted() },
        ),
        .unsigned_integer_overflow => std.fmt.bufPrintZ(
            &buffer,
            "negation of {} cannot be represented in type {}",
            .{ Value.init(data.type, old_value), data.type.fmtQuoted() },
        ),
        else => unreachable,
    } catch @trap();
    report(data.location, error_type, message);
}

const ShiftOutOfBoundsData = extern struct {
    location: SourceLocation,
    lhs_type: *const TypeDescriptor,
    rhs_type: *const TypeDescriptor,
};
export fn __ubsan_handle_shift_out_of_bounds(data: *const ShiftOutOfBoundsData, lhs: ValueHandle, rhs: ValueHandle) callconv(.C) void {
    const lhs_value = Value.init(data.lhs_type, lhs);
    const rhs_value = Value.init(data.rhs_type, rhs);
    const error_type: ErrorType = blk: {
        if (rhs_value.isNegative() or rhs_value.positiveInt() >= lhs_value.type.bitWidth()) {
            break :blk .invalid_shift_exponent;
        }
        break :blk .invalid_shift_base;
    };
    var buffer: [4096]u8 = undefined;
    const message = switch (error_type) {
        .invalid_shift_exponent => blk: {
            if (rhs_value.isNegative()) {
                break :blk std.fmt.bufPrintZ(&buffer, "shift exponent {} is negative", .{rhs_value});
            }
            break :blk std.fmt.bufPrintZ(&buffer, "shift exponent {} is too large for {}-bit type {}", .{ rhs_value, lhs_value.type.bitWidth(), lhs_value.type.fmtQuoted() });
        },
        .invalid_shift_base => blk: {
            if (lhs_value.isNegative()) {
                break :blk std.fmt.bufPrintZ(&buffer, "left shift of negative value {}", .{lhs_value});
            }
            break :blk std.fmt.bufPrintZ(&buffer, "left shift of {} by {} places cannot be represented in type {}", .{ lhs_value, rhs_value, lhs_value.type.fmtQuoted() });
        },
        else => unreachable,
    } catch @trap();
    report(data.location, error_type, message);
}

const OutOfBoundsData = extern struct {
    location: SourceLocation,
    array_type: *const TypeDescriptor,
    index_type: *const TypeDescriptor,
};
export fn __ubsan_handle_out_of_bounds(data: *const OutOfBoundsData, index: ValueHandle) callconv(.C) void {
    var buffer: [4096]u8 = undefined;
    const message = std.fmt.bufPrintZ(&buffer, "index {} out of bounds for type {}", .{ Value.init(data.index_type, index), data.array_type.fmtQuoted() }) catch @trap();
    report(data.location, .out_of_bounds_index, message);
}

/// TODO: Implement `__ubsan_handle_float_cast_overflow`
export fn __ubsan_handle_float_cast_overflow(data: *const anyopaque, from: ValueHandle) callconv(.C) void {
    _ = from;
    const location: *const SourceLocation = @ptrCast(@alignCast(data));
    report(location.*, .float_cast_overflow, "__ubsan_handle_float_cast_overflow");
}

const NonNullReturnData = extern struct {
    attr_location: SourceLocation,
};
/// TODO: Implement `__ubsan_handle_nonnull_return_v1`
/// TODO: Implement `__ubsan_handle_nullability_return_v1`
export fn __ubsan_handle_nonnull_return_v1(data: *const NonNullReturnData, location: *const SourceLocation) void {
    _ = data;
    report(location.*, .invalid_null_return, "__ubsan_handle_nonnull_return_v1");
}

const UnreachableData = extern struct {
    location: SourceLocation,
};
export fn __ubsan_handle_builtin_unreachable(data: *const UnreachableData) callconv(.C) void {
    report(data.location, .unreachable_call, "execution reached an unreachable program point");
}

extern "kernel32" fn OutputDebugStringA(lpOutputString: std.os.windows.LPCSTR) callconv(std.os.windows.WINAPI) void;

fn report(location: SourceLocation, error_type: ErrorType, message: [:0]const u8) void {
    var buffer: [4096]u8 = undefined;
    const formatted_message = std.fmt.bufPrintZ(&buffer, "UBSan({s}): {} - {s}\n", .{ @tagName(error_type), location.fmtFileName(), message }) catch @trap();
    OutputDebugStringA(formatted_message);
}
