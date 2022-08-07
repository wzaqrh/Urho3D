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

#include "../Container/HashMap.h"
#include "../Container/Ptr.h"

namespace Urho3D
{

class Animation;
class AnimatedModel;
class Deserializer;
class Serializer;
class Skeleton;
struct AnimationTrack;
struct Bone;

enum AnimationBlendMode
{
    ABM_LERP = 0,// Lerp blending (default)
    ABM_ADDITIVE// Additive blending based on difference from bind pose
};

struct AnimationStateTrack
{
    AnimationStateTrack();
    ~AnimationStateTrack();

    const AnimationTrack* track_;
    Bone* bone_;
    WeakPtr<Node> node_;

    float weight_;
    unsigned keyFrame_;
};

class URHO3D_API AnimationState : public RefCounted
{
public:
    AnimationState(AnimatedModel* model, Animation* animation);
    AnimationState(Node* node, Animation* animation);
    ~AnimationState();

    void SetStartBone(Bone* bone);/// Set start bone. Not supported in node animation mode. Resets any assigned per-bone weights.
    void SetBoneWeight(unsigned index, float weight, bool recursive = false);
    void SetBoneWeight(const String& name, float weight, bool recursive = false);
    void SetBoneWeight(StringHash nameHash, float weight, bool recursive = false);

	void SetLooped(bool looped);
	void SetWeight(float weight);
	void SetBlendMode(AnimationBlendMode mode);
    void SetLayer(unsigned char layer);

public://.setter
	void SetTime(float time);
    void AddTime(float delta);//.可能事件E_ANIMATIONFINISHED

	void AddWeight(float delta);

    void Apply();//.按time_调整node.localSRT

public://.getter
	float GetWeight() const { return weight_; }/// Return blending weight.
	float GetTime() const { return time_; }/// Return time position.

	bool IsEnabled() const { return weight_ > 0.0f; }/// Return whether weight is nonzero.
	bool IsLooped() const { return looped_; }/// Return whether looped.

    AnimationBlendMode GetBlendMode() const { return blendingMode_; }/// Return blending mode.
	float GetLength() const;/// Return animation length.
	unsigned char GetLayer() const { return layer_; }/// Return blending layer.

public://.getter
	Animation* GetAnimation() const { return animation_; }
	AnimatedModel* GetModel() const;
	Node* GetNode() const;
	Bone* GetStartBone() const;

	float GetBoneWeight(unsigned index) const;
	float GetBoneWeight(const String& name) const;
	float GetBoneWeight(StringHash nameHash) const;

	unsigned GetTrackIndex(Node* node) const;
	unsigned GetTrackIndex(const String& name) const;
	unsigned GetTrackIndex(StringHash nameHash) const;

private:
    void ApplyToModel();//所有track, 调用ApplyTrack, 权重为weight * track.weight, 静默
    void ApplyToNodes();//所有track, 调用ApplyTrack, 权重为100%
    void ApplyTrack(AnimationStateTrack& stateTrack, float weight, bool silent);//.按time_对track关键帧插值得到SRT, 按blendingMode_混合模式（权重weight）与node.localSRT混合

private:
    WeakPtr<AnimatedModel> model_;/// Animated model (model mode.)
    WeakPtr<Node> node_;/// Root scene node (node hierarchy mode.)
    SharedPtr<Animation> animation_;

    Bone* startBone_;//遍历animation_ tracks以name在startBone_里检索node、bone
    Vector<AnimationStateTrack> stateTracks_;//遍历animation_ tracks以name在node_里检索node
    
	float weight_;
	float time_;
    bool looped_;
    unsigned char layer_;
    AnimationBlendMode blendingMode_;
};

}
