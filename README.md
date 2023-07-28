websocket实现的聊天室功能，根据业务可以进行拓展

1、RoomManager是房间管理类，俗称业务类  
2、server是服务器类  
3、inc头文件、json.h暂未使用，使用是jsoncpp库，log日志类
4、server中on*函数，例如on_open函数，当客户端连接时会自动调用该函数  

大房间：所说的广播，client一链接就会创建大房间加入，client断开时退出

暂时未加入数据库服务  
已实现功能：登录、加入房间、离开房间、一对一发消息、房间内群发  
暂未实现功能：广播  


数据格式：

登录 
```
{
	"msgid": 1,
	"id": 11,
	"account": 20190 "password": "666"
}
```

加入房间
```
{
	"msgid": 1,
	"id": 12,
	"roomId": "zhihuishu"
}
```

离开房间 
```
{
	"msgid": 2,
	"id": 12,
	"leaveRoomId": "xxxx"
}
```

一对一聊天 
```
{
	"msgid": 5,
	"id": 11,
	"from": 11,
	"to": 12,
	"message": "这是一条测试消息"
}
```

房间内聊天 
```
{
	"msgid": 6,
	"id": 11,
	"message": "发送给某个房间的所有成员"
}
```

  