type ps
type pdf
type png
type pcl
type ss
conversion ps ps ps2ps -f -d
conversion pdf ps pdf2ps
conversion ps png ps2png
conversion png pcl png2pcl
conversion pcl ss pss
printer alice ps
printer bob pcl
