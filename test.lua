--
-- Created by IntelliJ IDEA.
-- User: alex
-- Date: 25/07/2015
-- Time: 15:35
-- To change this template use File | Settings | File Templates.
--

local LIBRARY_PATH = getenv("LIBRARY_PATH")
local ROOT_DIR = getenv("ROOT_DIR")

print("DIR:", ROOT_DIR)

handle, msg = loadlib(LIBRARY_PATH)

if (not handle or handle == -1) then
    error(msg)
end

callfromlib(handle, 'lua_ljsonopen')

function spaces(i)
    local s = ""
    while (i > 0) do
        s = s .. " "
        i = i - 1
    end
    return s
end

function printer(obj, key, level)
    if type(obj) == 'table' then
        if (key) then
            print(spaces(level) .. key .. ": {")
        else
            print(spaces(level) .."{")
        end
        foreach(obj, function(k, v)
            if type(v) == 'table' then
                printer(v, k, %level + 2)
            else
                print(spaces(%level + 3)..tostring(k) .. ": " .. tostring(v));
            end
        end)
        print(spaces(level) .. "},")
    else
        print(tostring(obj))
    end
end

function open_read(filepath)
    local file, msg = openfile(filepath, "rb")
    if (not file) then error(format("%s '%s'", msg, filepath)) end
    local data = read(file, ".*")
    closefile(file)
    return data
end

local i = 0
local prefix, filepath, decoded, encoded
while (i < 13) do
    print('==============================')
    if (i < 10) then
        prefix = format("000%i", i)
    else
        prefix = format("00%i", i)
    end

    filepath = ROOT_DIR..'/json-builder/test/valid-'..prefix..'.json'
    print(format('[decoded] %s', filepath))

    decoded = json_decode(open_read(filepath))
    printer(decoded, nil, 0)

    print('[encoded]')
    encoded = json_encode(decoded)
    print(encoded)

    i = i + 1
end