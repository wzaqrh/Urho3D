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

#include "../Math/BoundingBox.h"
#include "../Scene/Node.h"

namespace Urho3D
{

static const unsigned BONECOLLISION_NONE = 0x0;
static const unsigned BONECOLLISION_SPHERE = 0x1;
static const unsigned BONECOLLISION_BOX = 0x2;

class Deserializer;
class ResourceCache;
class Serializer;

struct Bone
{
    Bone() :
        parentIndex_(0),
        initialPosition_(Vector3::ZERO),
        initialRotation_(Quaternion::IDENTITY),
        initialScale_(Vector3::ONE),
        animated_(true),
        collisionMask_(0),
        radius_(0.0f)
    {}
    
    //.与关联node同名
    String name_;
    StringHash nameHash_;

    unsigned parentIndex_;

    //.TPose
    Vector3 initialPosition_;
    Quaternion initialRotation_;
    Vector3 initialScale_;

    /// Offset matrix.
    Matrix3x4 offsetMatrix_;
    /// Animation enable flag.
    bool animated_;
    /// Supported collision types.
    unsigned char collisionMask_;
    /// Radius.
    float radius_;
    /// Local-space bounding box.
    BoundingBox boundingBox_;
    /// Scene node.
    WeakPtr<Node> node_;
};

//.数据源
class URHO3D_API Skeleton
{
public:
    Skeleton();
    ~Skeleton();
	void ClearBones();
	void Define(const Skeleton& src);

	bool Load(Deserializer& source);
	bool Save(Serializer& dest) const;

    void Reset();//.重置node为TPose
	void ResetSilent();//.Reset但不标记脏
public:
    const Vector<Bone>& GetBones() const { return bones_; }
    Vector<Bone>& GetModifiableBones() { return bones_; }
    unsigned GetNumBones() const { return bones_.Size(); }

    Bone* GetRootBone();
    Bone* GetBone(unsigned index);
    Bone* GetBone(const String& boneName);
    Bone* GetBone(const char* boneName);
    Bone* GetBone(StringHash boneNameHash);
private:
    Vector<Bone> bones_;
    unsigned rootBoneIndex_;
};

}
