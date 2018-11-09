--HW4 Lua Example - Map Loder
--Stephen J. Guy, 2018

--This file has tabels which stores mappings that let us interpret the map file
--format that we are using: A-E are doors, a-e are keys, W's are walls, S is 
--start and G is goal. The function loadmap loads models and creates a 2D tabel
--that stores what is at each coordinate of the map.

-- What map symbols corrispond to doors
isDoor = {}
isDoor['A'] = true;
isDoor['B'] = true;
isDoor['C'] = true;
isDoor['D'] = true;

-- What map symbols corrispond to keys
isKey = {}
isKey['a'] = true;
isKey['b'] = true;
isKey['c'] = true;
isKey['d'] = true;

-- What map symbols corrispond to walls
isWall = {}
isWall['W'] = true;

--Map each type of door/key to a material
function setModelToMaterial(modelID, type)
  type = string.lower(type);
  if type == 'a' then 
    setModelMaterial(modelID,"Gold")
  elseif type == 'b' then
    setModelMaterial(modelID,"Steel")
  elseif type == 'c' then
    setModelMaterial(modelID,"Clay")
  elseif type == 'd' then
    setModelMaterial(modelID,"Shiny Red Plastic")
  elseif type == 'e' then
    setModelMaterial(modelID,"Polished Wood")
  end
end

-- check if a file exists
function file_exists(file)
  local f = io.open(file, "rb")
  if f then f:close() end
  return f ~= nil
end

-- get all lines from a file, returns an empty 
-- list/table if the file does not exist
function lines_from(file)
  if not file_exists(file) then 
    print("ERROR: File '"+ file + "' does not exist!")
    return {} 
  end
  local lines = {}
  for line in io.lines(file) do 
    lines[#lines + 1] = line --append line to lines array
  end
  return lines
end

local lines = {}

function loadMap(fileName)
  lines = lines_from(fileName)

  -- print all line numbers and their contents
  for k,v in pairs(lines) do
    print('line[' .. k .. ']', v)
  end

  mapSize = {}
  lines[1]:gsub("(%w+)", function(c) table.insert(mapSize,tonumber(c)) end)

  floorModel = addModel("Floor",0,0,0); --need to be able to scale this, maybe return model ID?
  --translateModel(floorModel,1,1,0)
  --translateModel(floorModel,.5,.5,-1)
  translateModel(floorModel,mapSize[2]/2+1,mapSize[1]/2+1,-.5)
  scaleModel(floorModel,mapSize[2],mapSize[1],.5)
  map = {}
  modelID = {}
  for i = 1, mapSize[2] do
    map[i] = {}
    modelID[i] = {}

    for j = 1, mapSize[1] do
      local c = lines[i+1]:sub(j,j)
      map[i][j] = c
      local id = -1
      
      local x = i
      local y = j
      local z = 0
      if isWall[c] then --Draw wall model in wall spot
        id = addModel("Wall",x+.5,y+.5,z);
      end
      if isDoor[c] then  --Draw door with right material
        id = addModel("Door",x+.5,y+.5,z)
        setModelToMaterial(id, c)
      end
      if isKey[c] then   --Draw key with material that matches door
        id = addModel("Key",x+.5,y+.5,z)
        setModelToMaterial(id, c)
        rotZVelModel[id] = .7 --Set the key spinning
        animatedModels[id] = true --We must add the item to the table of animated models
      end
      if c == "S" then  -- Player Start Location (set camera)
        CameraPosX = x+.5
        CameraPosY = y+.5
        caryKey = addModel("Key",CameraPosX,CameraPosY,z)
        placeModel(caryKey,0,0,-1) --hide active key model underground
      end
      if c == "G" then 
        id = addModel("Sphere",x+.5,y+.5,z);
        setModelMaterial(id,"Warm Light")
      end
      print("Map element:", x, y, z, c)

      modelID[i][j] = id
    end
  end

  --Make a backup of the map for level reloading
  savedMap = {}
  for i = 1, mapSize[2] do
    savedMap[i] = {}
    for j = 1, mapSize[1] do
      savedMap[i][j] = map[i][j]
    end
  end
end