# Image-Watermarker

g++ -o imagewatermark Imagewatermark.cpp -lboost_system -lboost_filesystem
-pthread -lX11

# 4 = nWorkers - 1000 = widthImages - 1000 = heightImages
# 1 - numero di cicli da eseguire sulle immagini, il resto dei parametri usa sempre quelli
./imagewatermark 4 ./Images ./ImageResults ./StampImage stampImage.jpg 
1000 1000 8
 
