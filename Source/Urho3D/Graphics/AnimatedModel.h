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

#include "../Graphics/Model.h"
#include "../Graphics/Skeleton.h"
#include "../Graphics/StaticModel.h"

namespace Urho3D
{

class Animation;
class AnimationState;

class URHO3D_API AnimatedModel : public StaticModel
{
    URHO3D_OBJECT(AnimatedModel, StaticModel);
    friend class AnimationState;
public:
    AnimatedModel(Context* context);
    virtual ~AnimatedModel();
    static void RegisterObject(Context* context);

    virtual bool Load(Deserializer& source, bool setInstanceDefault = false);
    virtual bool LoadXML(const XMLElement& source, bool setInstanceDefault = false);
    virtual bool LoadJSON(const JSONValue& source, bool setInstanceDefault = false);

    virtual void ApplyAttributes();//.应用延迟序列属性. 在 scene load | network update 之后调用
    virtual void ProcessRayQuery(const RayOctreeQuery& query, PODVector<RayQueryResult>& results);//.处理octree raycast, 工作线程安全
    virtual void Update(const FrameInfo& frame);//.在 octree再插入之前调用, 工作线程安全
    virtual void UpdateBatches(const FrameInfo& frame);/// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
    virtual void UpdateGeometry(const FrameInfo& frame);//.被View.UpdateGeometries回调
    virtual UpdateGeometryType GetUpdateGeometryType();//.返回geometry update是否必要, 是否工作线程安全
    
    virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);

    //.设置model_, 来自 手动调用 | E_RELOADFINISHED事件 | Load函数
    void SetModel(Model* model, bool createBones = true);
    
    //.animationStates_
    AnimationState* AddAnimationState(Animation* animation);
    void RemoveAnimationState(Animation* animation);
    void RemoveAnimationState(const String& animationName);
    void RemoveAnimationState(StringHash animationNameHash);
    void RemoveAnimationState(AnimationState* state);
    void RemoveAnimationState(unsigned index);
    void RemoveAllAnimationStates();
    void ApplyAnimation();//.更新node_ SRT、boneBoundingBox_, 变化来自animationStates_的time、weight
    
    //.morphs_权重
    void SetMorphWeight(unsigned index, float weight);
    void SetMorphWeight(const String& name, float weight);
    void SetMorphWeight(StringHash nameHash, float weight);
    void ResetMorphWeights();//.morph所有分量权重清零

	void SetAnimationLodBias(float bias);//.animation LOD bias
	void SetUpdateInvisible(bool enable);//.不可见时是否Update

    void UpdateBoneBoundingBox();//.可手动调用, 更新boneBoundingBox_

	//.序列属性
	void SetModelAttr(const ResourceRef& value);
	void SetBonesEnabledAttr(const VariantVector& value);
	void SetAnimationStatesAttr(const VariantVector& value);
	void SetMorphsAttr(const PODVector<unsigned char>& value);
public:
    Skeleton& GetSkeleton() { return skeleton_; }
    bool IsMaster() const { return isMaster_; }
    
    float GetAnimationLodBias() const { return animationLodBias_; }//.animation LOD bias
    bool GetUpdateInvisible() const { return updateInvisible_; }//.不可见时是否Update
    
	const Vector<PODVector<unsigned> >& GetGeometryBoneMappings() const { return geometryBoneMappings_;  }//.返回per-geometry bone mappings.
	const Vector<PODVector<Matrix3x4> >& GetGeometrySkinMatrices() const { return geometrySkinMatrices_; }//.返回per-geometry skin matrices. 为空时改用global skinning

	//.animationStates_
	const Vector<SharedPtr<AnimationState> >& GetAnimationStates() const { return animationStates_; }
	unsigned GetNumAnimationStates() const { return animationStates_.Size(); }
	AnimationState* GetAnimationState(Animation* animation) const;
	AnimationState* GetAnimationState(const String& animationName) const;
	AnimationState* GetAnimationState(const StringHash animationNameHash) const;
	AnimationState* GetAnimationState(unsigned index) const;

	//.morphs_ 与 morph vertex buffers_
	const Vector<ModelMorph>& GetMorphs() const { return morphs_; }
	unsigned GetNumMorphs() const { return morphs_.Size(); }
	float GetMorphWeight(unsigned index) const;
	float GetMorphWeight(const String& name) const;
	float GetMorphWeight(StringHash nameHash) const;
    const Vector<SharedPtr<VertexBuffer> >& GetMorphVertexBuffers() const { return morphVertexBuffers_; }

    //.序列属性
	ResourceRef GetModelAttr() const;
	VariantVector GetBonesEnabledAttr() const;
	VariantVector GetAnimationStatesAttr() const;
	const PODVector<unsigned char>& GetMorphsAttr() const;
protected:
    virtual void OnNodeSet(Node* node);///Handle node being assigned.
    virtual void OnMarkedDirty(Node* node);///Handle node transform being dirtied.
    virtual void OnWorldBoundingBoxUpdate();//.被GetWorldBoundingBox回调（脏标记）
private:
    void AssignBoneNodes();///Assign skeleton and animation bone node references as a postprocess. Called by ApplyAttributes
    void FinalizeBoneBoundingBoxes();///Finalize master model bone bounding boxes by merging from matching non-master bones.. Performed whenever any of the AnimatedModels in the same node changes its model.
    void RemoveRootBone();///Remove (old) skeleton root bone.

    void MarkAnimationDirty();//animation与skinning需要更新
    void MarkAnimationOrderDirty();//animation与skinning需要强制更新（blending order改变）
    void MarkMorphsDirty();//.morphs需要更新
    
    void SetSkeleton(const Skeleton& skeleton, bool createBones);/// Set skeleton.
    void SetGeometryBoneMappings();/// Set mapping of subgeometry bone indices.
    
    //.被UpdateGeometry回调
    void UpdateAnimation(const FrameInfo& frame);//.更新animationLodTimer_, 调用ApplyAnimation
    void UpdateMorphs();//.变化来自morphs_ weight, 更新morphVertexBuffers_
    void UpdateSkinning();//.变化来自bone.node.worldMat, 更新skinMatrices_

    void HandleModelReloadFinished(StringHash eventType, VariantMap& eventData);//.处理E_RELOADFINISHED事件
private:
	void CloneGeometries();
	void CopyMorphVertices(void* dest, void* src, unsigned vertexCount, VertexBuffer* clone, VertexBuffer* original);
    void ApplyMorph(VertexBuffer* buffer, void* destVertexData, unsigned morphRangeStart, const VertexBufferMorph& morph, float weight);
private:
	//.姿势相关
	Vector<SharedPtr<VertexBuffer> > morphVertexBuffers_;//.【pos、normal、tangent】, 原始数据来自model_的vertex buffer, 加上morphs_每一个（按权重）分量 //.覆盖geometries_原vertex buffer
	Vector<ModelMorph> morphs_;//.原始数据来自（除了weight）model_ morph
	unsigned morphElementMask_;//.morphs_ vertex buffer mask【pos、normal、tangent】
    mutable VectorBuffer attrBuffer_;//.主要是morphs_ weight

    //骨骼相关
    Skeleton skeleton_;
    BoundingBox boneBoundingBox_;//.所有bone的（相对node的）aabb并集
    Vector<SharedPtr<AnimationState>> animationStates_;

    PODVector<Matrix3x4> skinMatrices_;//.索引为boneId, 值为bone.node.worldMat * bone.offsetMat
    //.skinMatrices_复制多份给batches_使用, 索引为【boneId, batchId】
    //.used if more bones than skinning shader can manage.
	Vector<PODVector<unsigned>> geometryBoneMappings_;
	Vector<PODVector<Matrix3x4>> geometrySkinMatrices_;
	Vector<PODVector<Matrix3x4*>> geometrySkinMatrixPtrs_;

    //.LOD相关
    unsigned animationLodFrameNumber_;/// The frame number animation LOD distance was last calculated on.
    float animationLodBias_;/// Animation LOD bias.
    float animationLodTimer_;/// Animation LOD timer.
    float animationLodDistance_;/// Animation LOD distance, the minimum of all LOD view distances last frame.
    
    bool updateInvisible_;//.不可见时是否Update
    
    //.脏标记
    bool animationDirty_;
    bool animationOrderDirty_;
    bool morphsDirty_;
    bool skinningDirty_;
    bool boneBoundingBoxDirty_;

    bool isMaster_;//.是node的第一个AnimatedModel
    bool loading_;///Loading flag. During loading bone nodes are not created, as they will be serialized as child nodes.
    bool assignBonesPending_;///Bone nodes assignment pending flag.
    bool forceAnimationUpdate_;///Force animation update after becoming visible flag.
};

}
