extern fn __CxxFrameHandler(
    pExcept: ?*anyopaque,
    pRN: ?*anyopaque,
    pContext: ?*anyopaque,
    pDC: ?*anyopaque,
) callconv(.C) c_int;

export fn __CxxFrameHandler3(
    pExcept: ?*anyopaque,
    pRN: ?*anyopaque,
    pContext: ?*anyopaque,
    pDC: ?*anyopaque,
) callconv(.C) c_int {
    return __CxxFrameHandler(pExcept, pRN, pContext, pDC);
}

export fn __std_terminate() noreturn {
    @trap();
}
