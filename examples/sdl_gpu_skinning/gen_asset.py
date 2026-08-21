# Generates the demo asset (tentacle.fbx): a segmented cylinder skinned to a
# 3-bone armature with a keyframed sway animation. Run with:
#   blender -b -P gen_asset.py -- tentacle.fbx
# The committed FBX is produced by this script (no third-party asset; treat it
# as part of this repository under the repository license).
import math
import sys

import bpy

out_path = sys.argv[sys.argv.index("--") + 1] if "--" in sys.argv else "tentacle.fbx"

# clean scene
bpy.ops.wm.read_factory_settings(use_empty=True)

BONES = 3
BONE_LEN = 0.6
HEIGHT = BONES * BONE_LEN

# ---- mesh: a cylinder with enough height segments to bend ------------------
bpy.ops.mesh.primitive_cylinder_add(vertices=10, radius=0.22, depth=HEIGHT,
                                    location=(0, 0, HEIGHT / 2))
mesh_obj = bpy.context.active_object
mesh_obj.name = "Tentacle"
# subdivide lengthwise so skinning can bend it (kept coarse so the
# --wireframe mode is actually readable)
bpy.ops.object.mode_set(mode="EDIT")
bpy.ops.mesh.select_all(action="SELECT")
for _ in range(2):
    bpy.ops.mesh.subdivide()
bpy.ops.object.mode_set(mode="OBJECT")

# ---- armature: vertical 3-bone chain ---------------------------------------
bpy.ops.object.armature_add(enter_editmode=True, location=(0, 0, 0))
arm_obj = bpy.context.active_object
arm_obj.name = "Rig"
ebones = arm_obj.data.edit_bones
root = ebones[0]
root.name = "bone0"
root.head = (0, 0, 0)
root.tail = (0, 0, BONE_LEN)
prev = root
for i in range(1, BONES):
    b = ebones.new(f"bone{i}")
    b.head = (0, 0, i * BONE_LEN)
    b.tail = (0, 0, (i + 1) * BONE_LEN)
    b.parent = prev
    b.use_connect = True
    prev = b
bpy.ops.object.mode_set(mode="OBJECT")

# ---- skin with automatic weights --------------------------------------------
bpy.ops.object.select_all(action="DESELECT")
mesh_obj.select_set(True)
arm_obj.select_set(True)
bpy.context.view_layer.objects.active = arm_obj
bpy.ops.object.parent_set(type="ARMATURE_AUTO")

# ---- sway animation ----------------------------------------------------------
scene = bpy.context.scene
scene.frame_start = 1
scene.frame_end = 48
scene.render.fps = 24
bpy.context.view_layer.objects.active = arm_obj
bpy.ops.object.mode_set(mode="POSE")
for frame in range(1, 49):
    t = (frame - 1) / 48.0 * 2.0 * math.pi
    for i, pbone in enumerate(arm_obj.pose.bones):
        pbone.rotation_mode = "XYZ"
        pbone.rotation_euler = (math.sin(t + i * 0.9) * 0.45, 0.0,
                                math.cos(t * 2.0 + i * 0.5) * 0.15)
        pbone.keyframe_insert(data_path="rotation_euler", frame=frame)
bpy.ops.object.mode_set(mode="OBJECT")

# ---- export -------------------------------------------------------------------
bpy.ops.export_scene.fbx(filepath=out_path, use_selection=False,
                         add_leaf_bones=False, bake_anim=True,
                         bake_anim_use_nla_strips=False,
                         bake_anim_use_all_actions=False)
print(f"exported {out_path}")
