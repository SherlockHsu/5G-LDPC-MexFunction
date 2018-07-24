clear;

% Code block segmentation and code block CRC attachment 
% Ref: Section 5.2.1 in TS38.212

% Inputs and Ouputs
% input_bits: binary input bits, size 1xB
% output_bits: binary output bits, size CxK

% Parameters and Variables:
% B: the length of TBS, 'B' in TS38.212
% K: the length of code blocks after CBS, 'K' in TS38.212
% code_rate: the code rate, 'R' in TS38.212
% set_iLS: Sets of LDPC lifting size, Table 5.3.2-1 in TS38.212
% K_cb: maximum code block size, 'K_cb' in TS38.212
% K_p, K_n: 'K_+'and 'K_-' in TS38.212
% K_b: 'K_b' in TS38.212
% L: CRC lenth, 'L' in TS38.212
% C: number of code block after CBS, 'C' in TS38.212
% B1: B plus the lenght of CRC, 'B'' in TS38.212


B = 4600; 
code_rate = 0.5; 
K_cb = 8448; 

% %==============================================================
% Sets of LDPC lifting size Z
set_iLS =   [2  4   8   16  32  64  128 256 ...
             3  6   12  24  48  96  192 384 ...
             5  10  20  40  80  160 320 ...
             7  14  28  56  112 224 ...
             9  18  36  72  144 288 ...
             11 22  44  88  176 352 ...
             13 26  52  104 208 ...
             15 30  60  120 240];

input_bits = randi(2, 1, B)-1;
         
if (B <= K_cb)
    L = 0;
    C = 1;
    B1 = B;
else
    L = 24;
    C = ceil(B/(K_cb-L));
    B1 = B + C*L;
end

K_p = ceil(B1/C); % K_+ in the document
K_n = floor(B1/C);% K_- in the document

% % determine Z_c and K
if (code_rate > 0.67)
    base_graph_mode = 1;
    K_b = 22;
	Z_c = min(set_iLS(set_iLS*K_b - K_p >= 0));
    K = 22*Z_c;
else
	base_graph_mode = 2;
    if (B > 640)
        K_b = 10;
    else
        if (B>560)
            K_b=9;
        else
            if (B>192)
                K_b=8;
            else
                K_b=6;
            end
        end
    end
    
	Z_c = min(set_iLS(set_iLS*K_b - K_p >= 0));
    K = 10*Z_c;
    
    if (isempty(Z_c))
        base_graph_mode = 1;
        K_b = 22;
        Z_c = min(set_iLS(set_iLS*K_b - K_p >= 0));
        K = 22*Z_c;
    end
end

s = 0;
output_bits = -ones(C,K);

for r=0:C-1
    if r < mod(B,C)
        K1 = K_p;
    else
        K1 = K_n;
    end
    output_bits(r+1,1:K1-L) = input_bits(1,s+1:s+K1-L);
    s = s + K1-L;
    if (C > 1)
        output_bits(r+1,K1-L+1:K1) = zeros(1,L);
    end
end


