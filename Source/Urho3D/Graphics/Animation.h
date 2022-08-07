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

#include "../Container/Ptr.h"
#include "../Math/Quaternion.h"
#include "../Math/Vector3.h"
#include "../Resource/Resource.h"

namespace Urho3D
{

struct AnimationKeyFrame
{
    AnimationKeyFrame() :time_(0.0f), scale_(Vector3::ONE) {}

    float time_;

    //.SRT
    Vector3 position_;
    Quaternion rotation_;
    Vector3 scale_;
};

struct URHO3D_API AnimationTrack
{
    AnimationTrack() :channelMask_(0) {}

    void SetKeyFrame(unsigned index, const AnimationKeyFrame& command);
    void AddKeyFrame(const AnimationKeyFrame& keyFrame);
    void InsertKeyFrame(unsigned index, const AnimationKeyFrame& keyFrame);
    void RemoveKeyFrame(unsigned index);
    void RemoveAllKeyFrames();

    AnimationKeyFrame* GetKeyFrame(unsigned index);
    unsigned GetNumKeyFrames() const { return keyFrames_.Size(); }
    void GetKeyFrameIndex(float time, unsigned& index) const;

public:
    String name_;
    StringHash nameHash_;
    
    unsigned char channelMask_;//.mask SRT
    Vector<AnimationKeyFrame> keyFrames_;
};

struct AnimationTriggerPoint
{
    AnimationTriggerPoint() :time_(0.0f) {}

    float time_;
    Variant data_;
};

static const unsigned char CHANNEL_POSITION = 0x1;
static const unsigned char CHANNEL_ROTATION = 0x2;
static const unsigned char CHANNEL_SCALE = 0x4;

class URHO3D_API Animation : public ResourceWithMetadata
{
    URHO3D_OBJECT(Animation, ResourceWithMetadata);
public:
    Animation(Context* context);
    virtual ~Animation();
    static void RegisterObject(Context* context);

    virtual bool BeginLoad(Deserializer& source);
    virtual bool Save(Serializer& dest) const;
    SharedPtr<Animation> Clone(const String& cloneName = String::EMPTY) const;  

    void SetAnimationName(const String& name);
    void SetLength(float length);
    
    //.tracks_
    AnimationTrack* CreateTrack(const String& name);
    bool RemoveTrack(const String& name);
    void RemoveAllTracks();

    //.triggers_
    void SetTrigger(unsigned index, const AnimationTriggerPoint& trigger);
    void AddTrigger(const AnimationTriggerPoint& trigger);
    void AddTrigger(float time, bool timeIsNormalized, const Variant& data);
    void RemoveTrigger(unsigned index);
    void RemoveAllTriggers();
    void SetNumTriggers(unsigned num);  
public:
    //.name
    const String& GetAnimationName() const { return animationName_; }
    StringHash GetAnimationNameHash() const { return animationNameHash_; }

    float GetLength() const { return length_; }

    //.tracks_
    const HashMap<StringHash, AnimationTrack>& GetTracks() const { return tracks_; }
    unsigned GetNumTracks() const { return tracks_.Size(); }
    AnimationTrack *GetTrack(unsigned index);
    AnimationTrack* GetTrack(const String& name);
    AnimationTrack* GetTrack(StringHash nameHash);

    //.triggers_
    const Vector<AnimationTriggerPoint>& GetTriggers() const { return triggers_; }
    unsigned GetNumTriggers() const { return triggers_.Size(); }
    AnimationTriggerPoint* GetTrigger(unsigned index);

private:
    String animationName_;
    StringHash animationNameHash_;

    float length_;
    
    HashMap<StringHash, AnimationTrack> tracks_;
    Vector<AnimationTriggerPoint> triggers_;
};

}
