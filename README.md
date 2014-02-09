blender
=======

- An agnostic keyframe interpolation and animation controller. Written in C++11.
- MIT licensed

WIP WIP WIP WIP WIP

some theory
-----------

- An animation A is a collection of N keyframes
  A = { kf0, kf1, ... kfN }

- Blended frames are a mix of two keyframes.
  blended_frame = keyframe_A * factor + keyframe_B * (1-factor)

- Common case is using fixed blending factor of 0.5




some theory
-----------

- animacion a 60 hz (dt=16.5), 30 hz (dt=33)
- animation keyframes are render mandatory
- animation blended key are optional

- blending are done in odd numbers, during 1,3,5 [...] frames
- A

son obligatorias enseñarlas

- el frame rate
