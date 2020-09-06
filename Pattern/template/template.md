# 模板方法模式

---

基于继承的代码复用的基本应用之一。

```java
public abstract class Bicycle {

    private boolean isNeedUnlock = true;

    // 模板方法
    public final use(){
        if(isNeedUnlock){
            unlock();
        }else{
            System.out.println("锁已损坏，不需要解锁");
        }
        ride();
    }

    // 抽象方法1
    protected abstract void unlock();

    // 抽象方法2
    protected abstract void ride();

    // 钩子方法
    protected void isNeedUnlock(boolena isNeedUnlock){
        this.isNeedUnlock = isNeedUnlock;
    }
}
```

```java
public class BicycleScan extends Bicycle {

    @Override
    protected void unlock(){
        System.out.println("扫码开锁");
    }

    @Override
    protected void ride() {
        System.out.println("骑得很快");
    }

    // 运行结果：
    // 扫码开锁
    // 骑得很快
}
```

```java
public class BicycleLockBreaked extends Bicycle {

    @Override
    protected void unlock(){
        System.out.println("扫码开锁");
    }

    @Override
    protected void ride() {
        System.out.println("骑得很快");
    }

    // 重写钩子方法
    @Override
    pprotected void isNeedUnlock(boolena isNeedUnlock{
        this.isNeedUnlock = false;
    }

    // 运行结果：
    // 锁已损坏，不需要解锁
    // 骑得很快
}
```

模板方法模式的组成：

* 模板方法：负责対抽象方法进行调度，不可被重写
* 抽象方法：由子类实现， 由 protected 修饰，不需要暴露给其他类
* 钩子方法：参与到模板方法的调度中，提供默认的逻辑，也可被子类重写从而影响模板方法的部分调度逻辑，但一般不改变顶层的调度逻辑
