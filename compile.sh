ffmpeg -r 24 -f image2 -s 800x600 -i img/m%04d.png -vcodec libx264 -crf 25 -pix_fmt yuv420p test.mp4
