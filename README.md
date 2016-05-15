# SuperpixelRegionFill

Superpixels-based region filling

GitHub: [yhlleo/SuperpixelRegionFill](https://github.com/yhlleo/SuperpixelRegionFill)

抠取图像区域的一个小 demo，借助图像超像素分割的方法，将图像成子分块，再利用种子
填充算法，选取子块区域。

超像素分割方法，采用论文 *SEEDS: Superpixels Extracted via Energy-Driven
Sampling* 提出的方法，对于物体的边界具有较好的保留，如下图所示。可以辅助目标检
测中制作 Banchmark。

[![Star](http://atilimcetin.com/SEEDS/star_small.png)](http://atilimcetin.com/SEEDS/star.png)
[![Star Labels](http://atilimcetin.com/SEEDS/star_labels_small.png)](http://atilimcetin.com/SEEDS/star_labels.png)

GUI wrapper:

![](http://whudoc.qiniudn.com/2016/superfill-2.png)
![](http://whudoc.qiniudn.com/2016/superfill-3.png)
![](http://whudoc.qiniudn.com/2016/superfill-4.png)

想做成线跟踪，但是写出来的一个 demo，效果很差，不好用：

![](http://whudoc.qiniudn.com/2016/superfill-5.png)

种子填充算法使用递归的形式，对于小块的像素区域比较有效，但是由于编译器栈深度有
限，填充区域过大的时候，会出现栈溢出的 BUG，这里需要注意一下。

## References

-   [SEEDS: Superpixels Extracted via Energy-Driven Sampling](http://www.mvdblive.org/seeds/)
-   [SEEDS Superpixels Wrapper for OpenCV](https://github.com/yhlleo/SEEDS-superpixels)
