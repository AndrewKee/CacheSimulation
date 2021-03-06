close all
bzip2_inc_l1way_exectime = [53561312664 53792271746 54404036928];
bzip2_inc_l1way_ways     = [1 2 8];

% figure;
% plot (bzip2_inc_l1way_ways, bzip2_inc_l1way_exectime);
% title('bzip2 execution time changing l1 associations');
% xlabel('number of ways');
% ylabel('execution time in cycles');

executiontimes = [52170531595 27112120331 107260681086 82029355234 44300992452; ...
    51846279658 25335616586 107248844892 70284146604 40380511353; ...
    48769196313 24172730941 107286517495 60620863018 33993861102; ...
    48012706190 23224833703 108415187548 48032604706 28934606676; ... 
    53561312664 35147563146 108249525187 105194177165 53574291155; ...
    53792271746 29195493490 107410179423 89465410677 48127101144; ...
    54404036928 26897437126 107391534447 75995164217 44244084878; ...
    52917794978 38570299603 108285033462 122631067982 64377904161; ...
    52523407207 31722053479 107307940416 112567218800 58053201587; ...
    51004296206 25818912066 107249722874 75131696835 41225407974; ...
    51139234694 26830734120 108474926841 72913835611 39156215813];

cpitimes = [52170531595/10000000073 27112120331/10000000106 107260681086/16506492546 82029355234/10000000076 44300992452/10000000109; ...
            51846279658/10000000073 25335616586/10000000106 107248844892/16506492546 70284146604/10000000076 40380511353/10000000109; ...
            48769196313/10000000073 24172730941/10000000106 107286517495/16506492546 60620863018/10000000076 33993861102/10000000109; ...
            48012706190/10000000073 23224833703/10000000106 108415187548/16506492546 48032604706/10000000076 28934606676/10000000109; ... 
            53561312664/10000000073 35147563146/10000000106 108249525187/16506492546 105194177165/10000000076 53574291155/10000000109; ...
            53792271746/10000000073 29195493490/10000000106 107410179423/16506492546 89465410677/10000000076 48127101144/10000000109; ...
            54404036928/10000000073 26897437126/10000000106 107391534447/16506492546 75995164217/10000000076 44244084878/10000000109; ...
            52917794978/10000000073 38570299603/10000000106 108285033462/16506492546 122631067982/10000000076 64377904161/10000000109; ...
            52523407207/10000000073 31722053479/10000000106 107307940416/16506492546 112567218800/10000000076 58053201587/10000000109; ...
            51004296206/10000000073 25818912066/10000000106 107249722874/16506492546 75131696835/10000000076 41225407974/10000000109; ...
            51139234694/10000000073 26830734120/10000000106 108474926841/16506492546 72913835611/10000000076 39156215813/10000000109];

% associations = ['All 2 Way' 'All 4 Way' 'All FA' 'All FA L2 Big' 'Default' ...
%     'L1 2 Way' 'L1 8 Way' 'L1 Small' 'L1 Small 4 Way' 'L2 4 Way' ...
%     'L2 Big'];
% figure;
% bar(cpitimes);
% legend('bzip2', 'h264ref', 'libquantum', 'omnetpp', 'sjeng');
% xlabel('Associativities');
% ylabel('Execution Time (cycles)');

chunkcost = [525 625 725 825];
chunks = [53574291155 42804461615 37419546845 34727089460];
%put labels on the points
% figure;
% plot(chunkcost, chunks);
% xlabel('Cost of Configuration');
% ylabel('Execution Time (cycles)');

% chunksize = [8 16 32 64];
% figure;
% plot(chunksize, chunks);
% xlabel('Memory Chunk Size');
% ylabel('Execution Time (cycles)');

%configuration vs cost graph
% figure;
cost = [975 1425 4175 4775 525 925 1725 325 725 1025 975];
% bar(cost);
% ylabel('Cost');
% xlabel('Associativities');



%IPC
IPC = zeros(1, 11);
for i = 1:11
    IPC(i) = (sum(1./cpitimes(i, :))/5);
%     (sum(cpitimes(i, :))/5)
end
% figure;
% bar(IPC);
% ylabel('IPC');
% xlabel('Associativities');

%Omnetpp IPC
% figure;
omnIPC = 1./cpitimes(:,4);
% bar(omnIPC);
% title('omnetpp');
% ylabel('IPC');
% xlabel('Associativities');

% inrefto = 8;
% DeltaIPCperCost = zeros(1,11);
% for i = 1:11
%     DeltaIPCperCost(i) = (omnIPC(i)) / cost(i);
% end
% figure;
% bar(DeltaIPCperCost);
% title('Omnetpp');
% ylabel('IPC');
% xlabel('Associativities');
% 
% DeltaIPCDeltaCost = zeros(1,11);
% inrefto = 8;
% for i = 1:11
%     DeltaIPCDeltaCost(i) = (omnIPC(i) - omnIPC(inrefto)) / (cost(i) - cost(inrefto));
% end
% figure;
% bar(DeltaIPCDeltaCost);
% ylabel('IPC per Dollar');
% xlabel('Associativities');

%libquantum IPC
% figure;
% libIPC = 1./cpitimes(:,3);
% bar(libIPC);
% title('libquantum');
% ylabel('IPC');
% xlabel('Associativities');
% 
% inrefto = 11;
% DeltaIPCperCost = zeros(1,11);
% for i = 1:11
%     DeltaIPCperCost(i) = (libIPC(i)) / cost(i);
% end
% figure;
% bar(DeltaIPCperCost);
% title('libquantum');
% ylabel('IPC');
% xlabel('Associativities');


%average CPI (for each association) per dollar
IPCperdollar = zeros(1, 11);
for i = 1:11
    IPCperdollar(i) = (sum(1./cpitimes(i, :))/5)/cost(i);
%     (sum(cpitimes(i, :))/5)
end
% figure;
% bar(IPCperdollar);
% ylabel('IPC');
% xlabel('Associativities');

DeltaIPCDeltaCost = zeros(1,11);
inrefto = 8;
for i = 1:11
    DeltaIPCDeltaCost(i) = (IPC(i) - IPC(inrefto)) / (cost(i) - cost(inrefto));
    
    if i ~= inrefto
        
%         DeltaIPCDeltaCost(i) = (abs(IPCperdollar(inrefto) - IPCperdollar(i))/(cost(inrefto) - cost(i)));
    end
end
figure;
bar(DeltaIPCDeltaCost);
ylabel('IPC per Dollar');
xlabel('Associativities');

DeltaIPCDeltaCost = zeros(1,11);
inrefto = 8;
for i = 1:11
    DeltaIPCDeltaCost(i) = (IPC(i)) / (cost(i));
    
    if i ~= inrefto
        
%         DeltaIPCDeltaCost(i) = (abs(IPCperdollar(inrefto) - IPCperdollar(i))/(cost(inrefto) - cost(i)));
    end
end
% figure;
% bar(DeltaIPCDeltaCost);
% ylabel('IPC per Dollar');
% xlabel('Associativities');


avgCPI = zeros(1,11);
for i = 1:11
    avgCPI(i) = (sum(cpitimes(i, :))/5);
%     (sum(cpitimes(i, :))/5)
end

CPIperCost = zeros(1,11);

for i = 1:11
    CPIperCost(i) = (avgCPI(i)) * cost(i);
end
% figure;
% bar(CPIperCost);
% ylabel('CPI per Dollar');
% xlabel('Associativities');

