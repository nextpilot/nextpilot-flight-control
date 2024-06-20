function sl_customization(cm)
% cm = sl_customization_manager;
% cm.showWidgetIdAsToolTip=true;

cm.addCustomMenuFcn('Simulink:MenuBar', @item_addto_menubar);
cm.addCustomMenuFcn('Stateflow:MenuBar', @item_addto_menubar);

% 菜单有以下三种
% sl_container_schema
% sl_action_schema
% sl_toggle_schema

end
%% Define the custom menu function.
function schemaFcns = item_addto_menubar(callbackInfo)
schemaFcns = {
    @item_next_pilot,...
    %@getItem2,...
    %{@getItem3,3}... %% Pass 3 as user data to getItem3.
    %@getItem4
    };
end


function schema = item_next_pilot(callbackInfo)
schema = sl_container_schema;
schema.label = 'Nextpliot';
schema.childrenFcns = {
    @item_load_setting_data
    @item_edit_setting_file
    @item_create_bus_object
    %@getItem1
    };
end



%% Define the schema function for first menu item.
function schema = item_load_setting_data(callbackInfo)
schema = sl_action_schema;
schema.label = 'Load Setting Data ...';
schema.userdata = [];
schema.callback = @call_load_setting_data;
end

function call_load_setting_data(callbackInfo)
%nextpilot.simulink.read_project_setting([bdroot,'.xlsx']);
%msgbox(['success import seting file :"', bdroot, '.xlsx"'], 'Import Succeed');
read_model_config
end

function schema = item_edit_setting_file(callbackInfo)
schema = sl_action_schema;
schema.label = 'Edit Setting File ...';
schema.userdata = [];
schema.callback = @call_edit_setting_file;
end

function call_edit_setting_file(callbackInfo)
path = which(bdroot);
[path, name] = fileparts(path);
xlsx = fullfile(path, [name, '.xlsx']);
if ~exist(xlsx, 'file')
    sheet = {'genneral','config','struct','enum', 'param','signal'};
    for i = 1:length(sheet)
        xlswrite(xlsx,{}, sheet{i});
    end
end
winopen(xlsx);
end

function schema = item_create_bus_object(callbackInfo)
schema = sl_action_schema;
schema.label = 'Create Bus Object ...';
schema.userdata = [];
schema.callback = @call_create_bus_object;
end

function call_create_bus_object(callbackInfo)
Simulink.Bus.createObject(bdroot,gcbh);
end
