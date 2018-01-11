ffmpeg -r 24 -f image2 -s 1920x1080 -i img/m%04d.bmp -vcodec libx264 -crf 25 -pix_fmt yuv420p test.mp4
