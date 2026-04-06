# LKMLoader

纯内核实现的 lkm loader ，允许加载使用了非导出符号的内核模块，可作为 ksuinit 的替代。

## 用法

直接使用 insmod 即可加载，使用参数 module_path 指定要加载的 ko 路径（相对当前工作目录）。

```sh
insmod lkmloader.ko module_path=mylkm.ko
```

## 构建

项目包含本体 lkmloader ，以及一个示例 mylkm ，用于演示加载包含非导出符号的内核模块。

使用 ddk host 模式可以一次性构建所有 target 的产物：

```sh
./build-all.sh
```

输出到 `out/TARGET/{lkmloader.ko|mylkm.ko}`

[对于 6.12 内核，需要参考这个方法修正 Makefile.build](https://github.com/5ec1cff/ddk?tab=readme-ov-file#local-%E6%A8%A1%E5%BC%8F%E6%9E%84%E5%BB%BA%E9%80%82%E7%94%A8%E4%BA%8E%E5%A4%9A%E4%B8%AA-target-%E7%89%88%E6%9C%AC%E7%9A%84%E5%86%85%E6%A0%B8%E6%A8%A1%E5%9D%97)
