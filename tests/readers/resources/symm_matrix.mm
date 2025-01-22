%%MatrixMarket matrix coordinate real symmetric
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
%     0       0     10.5     0     0     0     3.0
%     0       0     0        0     0     0     0
%     10.5    0     -0.145   0     0     0     0
%     0       0     0        0     0     7.9   0
%     0       0     0        0     0     0     0
%     0       0     0        7.9   0     2.0   0
%     3.0     0     0        0     0     0     0.433
%
%=================================================================================
  7  7  6
    3     3  -1.450e-01 
    7     1   3.000e+00
    6     4   7.900e+00
    3     1   1.050e+01
    7     7   4.330e-01
    6     6   2.000e+00