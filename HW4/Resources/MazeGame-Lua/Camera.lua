--HW4 Lua Example - Camera
--Stephen J. Guy, 2018

--This module stores key camera parameters, and some camera helper functions.

CameraPosX = 2.0
CameraPosY = 0.0
CameraPosZ = .2

CameraDirX = 1.0
CameraDirY = 0.0
CameraDirZ = 0

CameraUpX = 0.0
CameraUpY = 0.0
CameraUpZ = 1.0

CameraAngle = math.atan2(CameraDirY,CameraDirX)

function setCamDirFromAngle(camAngle)
  CameraDirY = math.sin(camAngle)
  CameraDirX = math.cos(camAngle)
end

