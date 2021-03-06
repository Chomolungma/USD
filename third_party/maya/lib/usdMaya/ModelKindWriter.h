//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef PXRUSDMAYA_MODELKINDWRITER_H
#define PXRUSDMAYA_MODELKINDWRITER_H

/// \file ModelKindWriter.h

#include "pxr/pxr.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/stage.h"
#include "usdMaya/JobArgs.h"
#include "usdMaya/MayaPrimWriter.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE


/// This class encapsulates all of the logic for writing model kinds from
/// usdWriteJob. It is a "black box" that reads each newly-written prim, one
/// by one, saving information that is used to determine model hierarchy at the
/// end of writing to the USD stage.
class PxrUsdMaya_ModelKindWriter {
public:
    PxrUsdMaya_ModelKindWriter(const JobExportArgs& args);

    /// Processes the given prim in order to collect model hierarchy data.
    /// This should be called after the prim has been written with the given
    /// prim writer.
    /// Note: this assumes DFS traversal, i.e. parent prims should be traversed
    /// before child prims.
    void OnWritePrim(const UsdPrim& prim, const MayaPrimWriterPtr& primWriter);

    /// Writes model hierarchy for the given stage based on the information
    /// collected by OnWritePrim.
    /// Existing model hierarchy information will be verified if it already
    /// exists, and computed where it does not already exist.
    ///
    /// \returns true if the model hierarchy was written successfully, or
    ///          false if there was a problem verifying or writing model kinds
    bool MakeModelHierarchy(UsdStageRefPtr& stage);

    /// Clears all state, as if it were initiailly constructed.
    void Reset();

private:
    typedef std::unordered_map<SdfPath, std::vector<SdfPath>, SdfPath::Hash>
            SdfPathVectorMap;
    typedef std::unordered_map<SdfPath, bool, SdfPath::Hash> SdfPathBoolMap;
    typedef std::unordered_set<SdfPath, SdfPath::Hash> SdfPathSet;

    JobExportArgs _args;

    // Precomputes whether _args.rootKind IsA assembly.
    bool _rootIsAssembly;

    // Paths on at which we added USD references or authored kind.
    std::vector<SdfPath> _pathsThatMayHaveKind;

    // Maps root paths to list of exported gprims under the root path.
    // The keys in this map are only root prim paths that are assemblies
    // (either because _args.rootKind=assembly or kind=assembly was previously
    // authored).
    // These are just used for error messages.
    SdfPathVectorMap _pathsToExportedGprimsMap;

    // Set of all root paths that contain exported gprims.
    SdfPathSet _pathsWithExportedGprims;

    bool _AuthorRootPrimKinds(
            UsdStageRefPtr& stage,
            SdfPathBoolMap& rootPrimIsComponent);

    bool _FixUpPrimKinds(
            UsdStageRefPtr& stage,
            const SdfPathBoolMap& rootPrimIsComponent);
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXRUSDMAYA_MODELKINDWRITER_H
