# 香蕉派M2+温湿度记录服务器

由于使用了git submodule所以必须clone代码到本地。
	
	$ git clone https://github.com/bluebanboom/thserver.git
	$ cd thserver
	$ git submodule update --init --recursive

这样代码才完整。

将DHT11的data连接到M2+的11号引脚。

然后执行

	./bootstrap
	
命令完成后，访问[http://localhost:8080/status](http://localhost:8080/status)查看服务器运行状态。如果页面显示```Server running...｀``， 则表示执行正常。


bootstrap脚本创建了一个每小时执行一次的定时任务用来获取当前温度。待到收集到一定数据后可以打开[http://localhost:8080/static/index.html](http://localhost:8080/static/index.html)来查看当日的温湿度曲线。


