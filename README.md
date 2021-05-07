# SmartLock(基于安卓手机的智能门锁设计)
										
1.微控制器：STM32F103VET6


2.模块：4G通信模块、4*4矩阵键盘、AS608指纹识别模块


3.存储：FatFs文件系统  芯片：W25Q64


<---------认证密码、系统密码、一次性临时密码--------->


4.功能


A 指纹解锁（录、删、验证指纹）


* 刷指纹解锁：直接手指按压，检测指纹是否存在，存在即可开锁
* 录指纹：按一次11，需要成功输入认证密码方可录入。录入时首先验证指纹是否存在，若存在，建议更换指纹
* 删指纹：按一次12，输入ID方可删除
* 修改认证密码：按两次11，与原认证密码匹配成功，方可修改，修改后存储在文件系统中


B 密码解锁（系统密码、一次性临时密码）


* 系统密码解锁：按一次13，正确输入系统密码即可解锁
* 临时密码解锁：临时密码使用一次后失效，不使用则存储在文件系统
* 修改系统临时密码：按两次13，与原系统密码匹配成功，方可修改，修改后存储在文件系统中


C 手机APP


* 远程解锁
* 开门记录
* 用户管理（增、删用户，修改用户名）
* 设备管理（绑定设备、取消绑定、修改设备信息、查询设备、修改系统密码）
* 下发临时密码（有效次数一次）


D 通信


* 认证信息AES加密
* 每隔5秒发送心跳包维持连接
* 数据包格式：数据标识+数据内容（128位AES加密，ECB模式，数据填充方式pkcs5padding）+数据长度+CRC校验


NOTE


* 凡是解锁成功，设备都反馈给服务器，服务器反馈到手机用户
* 解锁成功：绿灯亮一秒熄灭；解锁失败：红灯亮一秒熄灭+蜂鸣器提示
* 手机APP的操作都是针对选中的设备进行操作
