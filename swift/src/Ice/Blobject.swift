//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

import Foundation

/// Base protocol for dynamic dispatch servants.
public protocol Blobject: Object {
    /// Dispatch an incoming request.
    ///
    /// - parameter inEncaps: `Data` - The encoded in-parameters for the operation.
    ///
    /// - parameter current: `Ice.Current` - The Current object to pass to the operation.
    ///
    /// - returns: A tuple with the following fields:
    ///
    ///   - ok: `Bool` - True if the operation completed successfully, false if
    ///     the operation raised a user exception (in this case, outParams
    ///     contains the encoded user exception). If the operation raises an
    ///     Ice run-time exception, it must throw it directly.
    ///
    ///   - outParams: `Data` - The encoded out-paramaters and return value
    ///     for the operation. The return value follows any out-parameters.
    func ice_invoke(inEncaps: Data, current: Current) throws -> (ok: Bool, outParams: Data)
}

public extension Blobject {
    func _iceDispatch(incoming inS: Incoming, current: Current) throws {
        let inEncaps = try inS.readParamEncaps()
        let invokeResult = try ice_invoke(inEncaps: inEncaps, current: current)
        inS.writeParamEncaps(ok: invokeResult.ok, outParams: invokeResult.outParams)
    }
}