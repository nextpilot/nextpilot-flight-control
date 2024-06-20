function read_uorb_msg(msg_list, save_to)
% uorb msg定义文件中包含有以下数据
% （1）结构体  ==> 转为Simulink.Bus
%  (2) 枚举量  ==> 转为Enum
arguments
    msg_list  cell {}  = "msg"
    save_to string {} = "base"
end


if ischar(msg_list)
    msg_list = {msg_list};
end

for i = 1 : length(msg_list)

    bus = {}; enum = {}; topic = {};

    file = msg_list{i};

    fid = fopen(file);
    while ~feof(fid)
        tline = strtrim(fgetl(fid));
        % 跳过空行和注释行
        if isempty(tline) || (startsWith(tline, "#") && ~startsWith(tline, "# TOPICS"))
            continue;
        end
        % 处理# TOPICS
        if startsWith(tline, "# TOPICS")
            topic =  strsplit(strrep(tline, "# TOPICS", ""));
            continue;
        end

        % 提取注释和正文
        tokens=regexpi(tline,'([^#]+)(.*)','tokens','once');
        left = tokens{1};
        comment = tokens{2};

        if contains(left,'=') % 枚举类型
            % uint8 AIRSPD_MODE_MEAS = 0	# airspeed is measured airspeed from sensor
            tok = regexpi(left, '(\w+)\s+(\w+)\s*=\s*([-\d]+)','tokens','once');
            enum(end+1, :) = {tok{2}, tok{1}, str2num(tok{3}), comment};
        else % 结构体
            % float32[3] vel_variance	# Variance in body velocity estimate
            tok=regexpi(tline,'(\w+)(\[\d+\])*\s+(\w+)','tokens','once');
            if isempty(tok{2})
                bus(end+1,:)={tok{3}, tok{1}, 1, comment};
            else
                bus(end+1,:)={tok{3}, tok{1}, str2num(tok{2}), comment};
            end
        end
    end
    fclose(fid);

end