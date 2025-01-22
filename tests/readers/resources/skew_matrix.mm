%%MatrixMarket matrix coordinate real skew-symmetric
%=================================================================================
%
% This ASCII file represents a sparse MxN matrix with L 
% nonzeros in the following Matrix Market format:
%
% +----------------------------------------------+
% |%%MatrixMarket matrix coordinate real general | <--- header line
% |%                                             | <--+
% |% comments                                    |    |-- 0 or more comment lines
% |%                                             | <--+         
% |    M  N  L                                   | <--- rows, columns, entries
% |    I1  J1  A(I1, J1)                         | <--+
% |    I2  J2  A(I2, J2)                         |    |
% |    I3  J3  A(I3, J3)                         |    |-- L lines
% |        . . .                                 |    |
% |    IL JL  A(IL, JL)                          | <--+
% +----------------------------------------------+   
%
% Indices are 1-based, i.e. A(1,1) is the first element.
%
% In this example the sparse matrix is the following :
% 
%     0       0       0        0       0.433    0       0     
%     0       37.5    0        0       0        2.16    0
%     0       0       0        0       0        0       0
%     0       0       0        0       0        0       -8.43
%     -0.433  0       0        0       -5.074   0       0
%     0       -2.16   0        0       0        0       0
%     0       0       0        8.43    0        0       0    
%
%=================================================================================
  7  7  5
    2     2   3.750e+01 
    7     4   8.430e+00
    5     1  -4.330e-01
    6     2  -2.160e+00
    5     5  -5.074e-00