# binary serach

## tips

* 循环条件的两种不同的写法：
    * left <= right 是在循环里直接找到目标值
    * left < right 是在循环里面是排除法，排除掉不是目标值的范围

## 不含重复元素的直接查找

* [33. 搜索旋转排序数组](https://leetcode.cn/problems/search-in-rotated-sorted-array/description/)

```java
public int search(int[] nums, int target) {
    int left = 0;
    int right = nums.length-1;
    while(left<=right){
        int mid = left+(right-left)/2;
        if(nums[mid]==target){
            return mid;
            // [left,mid] 有序
        }else if(nums[mid]>nums[right]){
            // target 是否在有序段，在就丢弃另一段
            if(nums[left]<=target && target<nums[mid]){
                right = mid-1;
            }else{
                left = mid+1;
            }
            // [mid,right] 有序
        }else if(nums[mid]<=nums[right]){
            if(nums[mid]<target && target<=nums[right]){
                left = mid+1;
            }else{
                right = mid-1;
            }
        }
    }
    return -1;
}
```

### mid 偏向性的引申

* 当 mid 偏左时，使用 right 位置元素比较可以不关心等号的归属

```java
int mid = left+(right-left)/2;
if(nums[mid]==target){

}else if(nums[mid]>nums[right]){
    
}else if(nums[mid]<=nums[right]){
    
}
```

```java
int mid = left+(right-left)/2;
if(nums[mid]==target){
   
}else if(nums[mid]>=nums[right]){
    
}else if(nums[mid]<nums[right]){
    
}
```

* 因为 mid 偏左时，只有两个元素时 left 和 mid 重合，所以和 left 位置元素比较时需要关心等号的归属

```java
int mid = left+(right-left)/2;
if(nums[mid]==target){
    
}else if(nums[mid]>=nums[left]){
    
}else if(nums[mid]<nums[left]){
    
}
```

```java
// case: [3,1] target=1 无法 ac
// 此时 left==mid==0，要找 1 就需要令 left=mid+1
// 而在 nums[left]==nums[mid] 的情况下 nums[left]<=target && target<nums[mid] 一定不成立，所以就需要让 left==mid==1 能走到第二个分支
int mid = left+(right-left)/2;
if(nums[mid]==target){
    
}else if(nums[mid]>nums[left]){
    
}else if(nums[mid]<=nums[left]){
    
}
```

* 同理，当 mid 偏右时，和 right 位置元素比较时需要关心等号的归属

```java
// case: [3,1] target=3 无法 ac
// 此时 mid==right==1，要找 3 就需要领 right=mid-1
// 而在 nums[mid]==nums[right] 的情况下 nums[mid]<target && target<=nums[right] 一定不成立，所以就需要让 mid==right==1 能走到第三个分支
int mid = left+(right-left+1)/2;
if(nums[mid]==target){
    
}else if(nums[mid]>=nums[right]){
    
}else if(nums[mid]<nums[right]){
    
}
```

* [74. 搜索二维矩阵](https://leetcode.cn/problems/search-a-2d-matrix/description/)

```java
public boolean searchMatrix(int[][] matrix, int target) {
    int m = matrix.length-1;
    int n = matrix[0].length-1;
    int left = 0;
    int right = m;
    while(left <= right){
        int mid = left+(right-left)/2;
        if(matrix[mid][0]>target){
            right = mid-1;
        }else if(matrix[mid][0]<target){
            left = mid+1;
        }else if(matrix[mid][0]==target){
            return true;
        }
    }
    if(left==0){
        return false;
    }
    int row = left-1;
    // while(left <= right){
    //     int mid = left+(right-left)/2;
    //     if(matrix[mid][n]>target){
    //         right = mid-1;
    //     }else if(matrix[mid][n]<target){
    //         left = mid+1;
    //     }else if(matrix[mid][n]==target){
    //         return true;
    //     }
    // }
    // if(left>m){
    //     return false;
    // }
    // int row = left;
    left = 0;
    right = n;
    while(left <= right){
        int mid = left+(right-left)/2;
        if(matrix[row][mid]==target){
            return true;
        }else if(matrix[row][mid]>target){
            right = mid-1;
        }else{
            left = mid+1;
        }
    }
    return false;
}
```

* 本题可以看成「在列上的无重复元素的查找」+「在行上含重复元素的查找」
* 列上的无重复元素查找可以选第一列，也可选最后一列，区别是对越界情况处理不同
* 循环终止时 left==right+1，所以 left 取值范围为 [0,nums.length]
    * 选第一列比较时
        * 如果 left==0，则 target 比所有数都小
        * 否则 target 在 left-1 行

## 含重复元素的直接查找

* [81. 搜索旋转排序数组 II](https://leetcode.cn/problems/search-in-rotated-sorted-array-ii/description/)

```java
public boolean search(int[] nums, int target) {
    int left = 0;
    int right = nums.length-1;
    while(left<=right){
        int mid = left+(right-left)/2;
        if(nums[mid]==target){
            return true;
        }else if(nums[mid]>nums[right]){
            if(nums[left]<=target && target<nums[mid]){
                right=mid-1;
            }else{
                left=mid+1;
            }
        }else if(nums[mid]<nums[right]){
            if(nums[mid]<target && target<=nums[right]){
                left=mid+1;
            }else{
                right=mid-1;
            }
        }else if(nums[mid]==nums[right]){
            right--;
        }
    }
    return false;
}
```

* mid 偏左时收缩右边界，因为和 right 位置元素已经比较过了，right 位置肯定不是 target
* mid 偏右时收缩左边界，因为 left 位置元素已经比较过了，left 位置肯定不是 target

## 不含重复元素的边界查找

* [153. 寻找旋转排序数组中的最小值](https://leetcode.cn/problems/find-minimum-in-rotated-sorted-array/description/)

```java
public int findMin(int[] nums) {
    int left = 0;
    int right = nums.length-1;
    while(left<=right){
        int mid = left+(right-left)/2;
        if(nums[mid]>nums[nums.length-1]){
            left = mid+1;
        }else if(nums[mid]<nums[nums.length-1]){
            right = mid-1;
        }else if(nums[mid]==nums[nums.length-1]){
            right = mid-1;
        }
    }
    return nums[left];
}
```

* 根据题目规定的旋转方式，最小值是右边元素的左边界
* 由于查找的始终是 [n,nums.lenght-1] 的左边界，所以每次都是和 nums.length-1 处的元素相比
* 由于是查找左边界，所以相等时收缩右边界
* 由于最后一次循环时 left==right==mid，那么退出循环时 right 变为 mid-1，所以最终需要返回 left 处的元素

## 含重复元素的边界查找

* [154. 寻找旋转排序数组中的最小值 II](https://leetcode.cn/problems/find-minimum-in-rotated-sorted-array-ii/description/)

```java
public int findMin(int[] nums) {
    int left = 0;
    int right = nums.length-1;
    while(left<=right){
        int mid = left+(right-left)/2;
        if(nums[mid]>nums[right]){
            left = mid+1;
        }else if(nums[mid]<nums[right]){
            right = mid;
        }else if(nums[mid]==nums[right]){
            right--;
        }
    }
    return nums[left];
}
```

* 和 153 的两个区别
    * 包含重复元素，所以不再是查找右边固定的 [n,nums.lenght-1] 的左边界，所以需要和 right 位置元素比较
    * mid < right 时，mid 可能就是最小元素，所以下次查找的区间就是 [left,mid]
* 在循环条件可以带等号时，此题中也出现了 right=mid 的逻辑，因为此题中当 right==mid 后 right 还会修改，所以不会造成死循环

* [34. 在排序数组中查找元素的第一个和最后一个位置](https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array/description/)

```java
public int[] searchRange(int[] nums, int target) {
    if(nums.length==0){
        return new int[]{-1,-1};
    }
    int[] result = new int[2];
    result[0] = searchLeft(nums,target);
    result[1] = searchRight(nums,target);
    return result;
}

private int searchLeft(int[] nums,int target){
    int left = 0;
    int right = nums.length-1;
    while(left<=right){
        int mid = left+(right-left)/2;
        System.out.println("searchLeft:left="+left+",mid="+mid+",right="+right);
        if(nums[mid]>target){
            right = mid-1;
        }else if(nums[mid]<target){
            left = mid+1;
        }else if(nums[mid]==target){
            right = mid-1;
        }
    }
    if(left>=nums.length || nums[left]!=target){
        return -1;
    }
    return left;
}

private int searchRight(int[] nums,int target){
    int left = 0;
    int right = nums.length-1;
    while(left<=right){
        int mid = left+(right-left)/2;
        System.out.println("searchRight:left="+left+",mid="+mid+",right="+right);
        if(nums[mid]>target){
            right = mid-1;
        }else if(nums[mid]<target){
            left = mid+1;
        }else if(nums[mid]==target){
            left = mid+1;
        }
    }
    if(right<0 || nums[right]!=target){
        return -1;
    }
    return right;
}
```

* 本题中不存在 mid 和 left or right 的比较，所以不用关注 mid 的偏向性
* 搜索左边界时的额外判断条件
    * target 比所有元素大时，left 会越界
    * target 比所有元素小时，right 会越界，退出循环，而此时 nums[left] 不是 target
* 循环终止的条件一定是 left == right+1，因为 left==right==mid 时，要么 left+1，要么 right-1，无论怎样下一步一定是 left == right+1

## 不含重复元素的极值查找

* [162. 寻找峰值](https://leetcode.cn/problems/find-peak-element/description/)

```java
public int findPeakElement(int[] nums) {
    int left = 0;
    int right = nums.length-1;
    while(left<=right){
        int mid = left+(right-left)/2;
        if(mid==right){
            return left;
        }
        if(nums[mid]<nums[mid+1]){
            left = mid+1;
        }else if(nums[mid]>nums[mid+1]){
            right = mid;
        }
    }
    return left;
```

* 由于是局部最大值，所以 mid > mid+1 时 mid 可能就是极值，所以 right=mid
* 由于循环条件含等号，并且存在 right=mid，所以为了避免死循环，必须判断 left==mid==right 时的 case