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

#include "../IO/VectorBuffer.h"
#include "../Scene/Component.h"
#include "../Graphics/AnimationState.h"

namespace Urho3D
{

class AnimatedModel;
class Animation;
struct Bone;

struct URHO3D_API AnimationControl
{
    AnimationControl() :
        speed_(1.0f),
        targetWeight_(0.0f),
        fadeTime_(0.0f),
        autoFadeTime_(0.0f),
        setTimeTtl_(0.0f),
        setWeightTtl_(0.0f),
        setTime_(0),
        setWeight_(0),
        setTimeRev_(0),
        setWeightRev_(0),
        removeOnCompletion_(true)
    {}

    //.name, 用于查找animationState
    String name_;
    StringHash hash_;
    
    float speed_;           //.播放倍数
    float targetWeight_;    //.淡入淡出目标权重
    float fadeTime_;        //.淡入淡出时间
    float autoFadeTime_;    //.播放结束后, 淡出时间
    
    //.command
    float setTimeTtl_;       /// Set time command time-to-live.
    float setWeightTtl_;     /// Set weight command time-to-live.
    unsigned short setTime_; /// Set time command.
    unsigned char setWeight_;/// Set weight command.
    
    //.command revision
    unsigned char setTimeRev_;  /// Set time command revision.
    unsigned char setWeightRev_;/// Set weight command revision.

    bool removeOnCompletion_;   /// Sets whether this should automatically be removed when it finishes playing.
};

class URHO3D_API AnimationController : public Component
{
    URHO3D_OBJECT(AnimationController, Component);
public:
    AnimationController(Context* context);
    virtual ~AnimationController();
    static void RegisterObject(Context* context);

    virtual void OnSetEnabled();
    virtual void Update(float timeStep);//更新animation states weight, 变化来自animation states time

public:
    bool Play(const String& name, unsigned char layer, bool looped, float fadeInTime = 0.0f);//.会从组件ResourceCache Animation（参数name）读取Animation，权重100%
    bool PlayExclusive(const String& name, unsigned char layer, bool looped, float fadeTime = 0.0f);//.调用Play, 淡出同一层其他动画

	bool Fade(const String& name, float targetWeight, float fadeTime);//.淡出或淡入动画
	bool FadeOthers(const String& name, float targetWeight, float fadeTime);//.淡出或淡入同一层其他动画

    bool Stop(const String& name, float fadeOutTime = 0.0f);//.等价于Fade targetWeight=0
    void StopLayer(unsigned char layer, float fadeOutTime = 0.0f);
    void StopAll(float fadeTime = 0.0f);

    bool SetLayer(const String& name, unsigned char layer);
    bool SetStartBone(const String& name, const String& startBoneName);
    bool SetTime(const String& name, float time);
    bool SetWeight(const String& name, float weight);
    bool SetLooped(const String& name, bool enable);
    bool SetSpeed(const String& name, float speed);
    bool SetAutoFade(const String& name, float fadeOutTime);
    bool SetRemoveOnCompletion(const String& name, bool removeOnCompletion);
    bool SetBlendMode(const String& name, AnimationBlendMode mode);

    //.序列属性
    void SetAnimationsAttr(const VariantVector& value);
    void SetNetAnimationsAttr(const PODVector<unsigned char>& value);
    void SetNodeAnimationStatesAttr(const VariantVector& value);

public:
	bool IsPlaying(const String& name) const;
	bool IsPlaying(unsigned char layer) const;
	bool IsFadingIn(const String& name) const;
	bool IsFadingOut(const String& name) const;
	bool IsAtEnd(const String& name) const;

	unsigned char GetLayer(const String& name) const;
	Bone* GetStartBone(const String& name) const;
	const String& GetStartBoneName(const String& name) const;
	float GetTime(const String& name) const;
	float GetWeight(const String& name) const;

	bool IsLooped(const String& name) const;
	AnimationBlendMode GetBlendMode(const String& name) const;
	float GetLength(const String& name) const;
	float GetSpeed(const String& name) const;

	float GetFadeTarget(const String& name) const;
	float GetFadeTime(const String& name) const;
	float GetAutoFade(const String& name) const;

	bool GetRemoveOnCompletion(const String& name) const;
	AnimationState* GetAnimationState(const String& name) const;
	AnimationState* GetAnimationState(StringHash nameHash) const;
	const Vector<AnimationControl>& GetAnimations() const { return animations_; }

    //.序列属性
    VariantVector GetAnimationsAttr() const;
    const PODVector<unsigned char>& GetNetAnimationsAttr() const;
    VariantVector GetNodeAnimationStatesAttr() const;

protected:
    /// Handle scene being assigned.
    virtual void OnSceneSet(Scene* scene);

private:
    AnimationState* AddAnimationState(Animation* animation);
    void RemoveAnimationState(AnimationState* state);
    void FindAnimation(const String& name, unsigned& index, AnimationState*& state) const;
    
    void HandleScenePostUpdate(StringHash eventType, VariantMap& eventData); /// Handle scene post-update event.

private:
    Vector<AnimationControl> animations_;
    Vector<SharedPtr<AnimationState>> nodeAnimationStates_; /// Node hierarchy mode animation states.
    mutable VectorBuffer attrBuffer_; /// Attribute buffer for network replication.
};

}
