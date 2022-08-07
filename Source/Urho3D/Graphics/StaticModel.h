//
// Copyright (c) 2008-2017 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Graphics/Drawable.h"

namespace Urho3D
{

class Model;

/// Static model per-geometry extra data.
struct StaticModelGeometryData
{
    /// Geometry center.
    Vector3 center_;
    /// Current LOD level.
    unsigned lodLevel_;
};

/// Static model component.
class URHO3D_API StaticModel : public Drawable
{
    URHO3D_OBJECT(StaticModel, Drawable);

public:
    StaticModel(Context* context);
    ~StaticModel();
    static void RegisterObject(Context* context);

    /// Process octree raycast. May be called from a worker thread.
    virtual void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);
    /// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
    virtual void UpdateBatches(const FrameInfo& frame);
    /// Return the geometry for a specific LOD level.
    virtual Geometry* GetLodGeometry(unsigned batchIndex, unsigned level);
    /// Return number of occlusion geometry triangles.
    virtual unsigned GetNumOccluderTriangles();
    /// Draw to occlusion buffer. Return true if did not run out of triangles.
    virtual bool DrawOcclusion(OcclusionBuffer* buffer);

    virtual void SetModel(Model* model);//设置model_, 来自用户调用|E_RELOADFINISHED事件|Load函数

    /// Set material on all geometries.
    virtual void SetMaterial(Material* material);
    /// Set material on one geometry. Return true if successful.
    virtual bool SetMaterial(unsigned index, Material* material);
    /// Set occlusion LOD level. By default (M_MAX_UNSIGNED) same as visible.
    void SetOcclusionLodLevel(unsigned level);
    /// Apply default materials from a material list file. If filename is empty (default), the model's resource name with extension .txt will be used.
    void ApplyMaterialList(const String& fileName = String::EMPTY);

	//.序列属性：Model、Material
	void SetModelAttr(const ResourceRef& value);
	void SetMaterialsAttr(const ResourceRefList& value);
public:
	Model* GetModel() const { return model_; }
	virtual Material* GetMaterial(unsigned index = 0) const;

    unsigned GetNumGeometries() const { return geometries_.Size(); }
    unsigned GetOcclusionLodLevel() const { return occlusionLodLevel_; }

	bool IsInside(const Vector3& point) const;//.返回world point是否在geometry_内
	bool IsInsideLocal(const Vector3& point) const;//.返回local point是否在geometry_内

    //.序列属性：Model、Material
    ResourceRef GetModelAttr() const;
    const ResourceRefList& GetMaterialsAttr() const;
protected:
    virtual void OnWorldBoundingBoxUpdate();//.被GetWorldBoundingBox回调（脏标记）
    /// Set local-space bounding box.
    void SetBoundingBox(const BoundingBox& box);
    /// Set number of geometries.
    void SetNumGeometries(unsigned num);
    /// Reset LOD levels.
    void ResetLodLevels();
    /// Choose LOD levels based on distance.
    void CalculateLodLevels();

    /// Extra per-geometry data.
    PODVector<StaticModelGeometryData> geometryData_;
    /// All geometries.
    Vector<Vector<SharedPtr<Geometry> > > geometries_;
    
    SharedPtr<Model> model_;//.数据源

    /// Occlusion LOD level.
    unsigned occlusionLodLevel_;
    /// Material list attribute.
    mutable ResourceRefList materialsAttr_;
private:
    /// Handle model reload finished.
    void HandleModelReloadFinished(StringHash eventType, VariantMap& eventData);
};

}
