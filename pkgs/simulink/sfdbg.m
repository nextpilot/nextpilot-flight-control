function sfdbg(s)
coder.extrinsic('fprintf');
persistent cnt lst
if isempty(cnt)
    cnt = 1;
    lst = 0;
end
now = getSimulationTime();

if lst ~= now
    cnt = 1;
    fprintf('----------------------------------\n');    
end
fprintf('[%gs][%d]%s\n',now,cnt,s);
lst = now;
cnt = cnt + 1;
 
