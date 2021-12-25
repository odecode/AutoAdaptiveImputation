Implementation of a collaborative filtering research paper.
Citation: Yongli Ren, Gang Li, Jun Zhang, and Wanlei Zhou. 2012. The efficient imputation method for neighborhood-based collaborative filtering. In Proceedings of the 21st ACM international conference on Information and knowledge management (CIKM '12). Association for Computing Machinery, New York, NY, USA, 684–693. DOI:https://doi.org/10.1145/2396761.2396849
Included:
-the paper
-MovieLens benchmarking data file (MovieLens 100k)
-c++ implementation code

In branch vcl I have a vectorized version using Agner Fog's vector class library (https://www.agner.org/optimize/#vectorclass). Unzip to this projects code directory, add the vector class folder to include path, and compile with g++ -O3 -std=c++17 -I./version2-2.01.04 newmain.cpp -o newmain (or with whatever version you downloaded)
