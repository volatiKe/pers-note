# 单例模式

---

## 1. 饿汉式

```java
class Single{

    private static Single s = new Single();

    private Single(){}

    public static Single getInstance(){
        return s;
    }
}
```

线程安全，单例没有使用就会造成资源浪费。

## 2. 懒汉式

```java
class Single{

    private static Single s;

    private Single(){}

    public static (synchronized) Single getInstance()
    {
        if(s==null){
            s = new Single();
        }
        return s;
    }
}
```

* 并发下多个线程进入 if 时会创建多个实例
* 通过加锁可防止并发问题，但是存在性能问题

## 3. 懒汉式：Double Check

```java
class Single{

    private static volitale s;

    private Single(){}

    public static Single getInstance(){
        if(s==null){
            synchronized(Single.class){
                if(s==null){
                    s = new Single();
                }
            }
        }
        return s;
    }
}
```

* 相比于饿汉式减小了锁的粒度
* 一次非空判断避免在实例化完成后対锁的判断
* 二次非空判断避免了并发导致多个实例
* volatile 避免了[指令重排](../../Java/memory_model/memory_model.md#ordered)

## 4. 懒汉式：静态内部类

```java
class Single{
    private Single(){}

    private static class InnerSingle{
        private static final Single SINGLE = new Single();
    }

    public static Single getInstance(){
        return InnerSingle.SINGLE;
    }
}
```

* 这个形式的懒加载体现在静态内部类不会随着外部类的初始化而被初始化，而是当访问 getInstance() 时需要产生対 InnerSingle 的静态属性的引用时才会触发対内部类的初始化
* 是线程安全的，因为类加载的初始化阶段是单线程的
* 弊端是无法将外部参数传递给内部类

## 5. 枚举类

```java
public enum Singleton {  

    INSTANCE;

    public void whateverMethod() {  
    }  
}
```

以上所有的实现方式中，均可以通过反射的方式打破构造方法的私有限制，序列化也会创造出单例的副本，因为每次调用 readObject() 返回的都是一个新创建出来的对象。

* 避免反射：
  * 当使用 Single.class.getDeclaredConstructor() 试图获取构造方法时会抛出异常，提示没有无参的构造方法，因为枚举类继承 Enum，Enum 没有空参构造方法
  * 当使用 Single.class.getDeclaredConstructor 获取 Enum 的有参构造方法（String.class,int.class），仍然会抛出异常，提示拿到了构造方法，但是无法反射，因为反射的 newInstance() 在判断为枚举类时禁止反射
* 枚举类自行处理序列化，不会破坏单例
