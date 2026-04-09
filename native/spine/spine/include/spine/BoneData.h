/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated July 28, 2023. Replaces all prior versions.
 *
 * Copyright (c) 2013-2023, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software or
 * otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THE
 * SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef SPINE_BONEDATA_H_
#define SPINE_BONEDATA_H_

#include <spine/dll.h>
#include <spine/Color.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SP_INHERIT_NORMAL,
	SP_INHERIT_ONLYTRANSLATION,
	SP_INHERIT_NOROTATIONORREFLECTION,
	SP_INHERIT_NOSCALE,
	SP_INHERIT_NOSCALEORREFLECTION
} spInherit;

typedef struct spBoneData spBoneData;
struct spBoneData {
	int index;
	char *name;
	spBoneData *parent;

	/**
	 * 骨骼长度，用处
	 * 1、IK约束计算时使用
	 * 2、路径约束时使用 
	 * 3、在编辑器中由编辑器自动计算权重时使用（根据骨骼长度来计算骨骼对顶点影响的百分比）
	 */
	float length;

	//Spine的坐标系是：原点为屏幕中心，向右为x轴正轴，向上为y轴正轴
	//存储的为相对于父节点的本地坐标
	float x, y, scaleX, scaleY, shearX, shearY;

	/**
	 * 存储的为相当于父节点的本地旋转
	 * 1、0向右，90向上，180向左，270向下
	 * 2、存储的是逆时针数值。即逆时针为正值。范围为0~360
	 */
	float rotation;

	/**
	 * 表示当前节点的变换如果继承父节点的变换
	 * 比如可以忽略继承父节点的旋转，那么可以设置为SP_INHERIT_NOROTATIONORREFLECTION
	 */
	spInherit inherit;
	int/*bool*/ skinRequired;

	//对应在编辑器中为骨骼设置的颜色，不渲染的
	spColor color;
    const char *icon;

	/**
	 * 在骨架中我们可以设置显示或隐藏此骨骼
	 */
    int/*bool*/ visible;
};

SP_API spBoneData *spBoneData_create(int index, const char *name, spBoneData *parent);

SP_API void spBoneData_dispose(spBoneData *self);

#ifdef __cplusplus
}
#endif

#endif /* SPINE_BONEDATA_H_ */
