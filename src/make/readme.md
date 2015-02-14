Makefile�м�����gcc,g++�ĺܶ�ѡ���һЩ˵��


-Wpointer-arith ѡ��
5.17 Arithmetic on void- and Function-Pointers
In GNU C, addition and subtraction operations are supported on pointers to void and on pointers to functions.
This is done by treating the size of a void or of a function as 1.
A consequence of this is that sizeof is also allowed on void and on function types, and returns 1.
The option -Wpointer-arith requests a warning if these extensions are used.
ע���������-Wall����



-fvisibility=hidden ѡ��
��windows�У����ǿ���ָ��__declspec(dllexport)���������Ƶ�����������linux�£�����Ҳ�����ƵĿ��Ʋ�����

��GCC�����ĵ� -fvisibility=default|internal|hidden|protected������������һ��������

a superior solution made possible by this option to marking things 
hidden when the default is public is to make the default hidden and 
mark things public. This is the norm with DLL's on Windows and with 
-fvisibility=hidden and "__attribute__ ((visibility("default")))" 
instead of "__declspec(dllexport)" you get almost identical semantics 
with identical syntax. This is a great boon to those working with cross-platform projects.

��Ҫ�˽���ǣ���linux�£�Դ�ļ��е����к�������һ��Ĭ�ϵ�visibility����Ϊpublic���ڱ��������м��� -fvisibility=hidden������
�Ὣ����Ĭ�ϵ�public�����Ա�Ϊhidden����ʱ������Ժ�������__attribute__ ((visibility("default")))������
ʹ�ض��ĺ�����Ȼ��Ĭ�ϵ�public���Դ�����-fvisibility=hidden��������Ըú��������á�
���ԣ�������-fvisibility=hidden����֮��ֻ��������__attribute__ ((visibility("default")))�ĺ������Ƕ���ɼ��ģ�
�����Ч����ͬ��Visual Studio�µ�__declspec(dllexport)���塣




gcc �� -g �� -ggdb ѡ��

-g �� -ggdb ������ gcc ���ɵ�����Ϣ����������Ҳ���������

-g
��ѡ��������ò���ϵͳ�ġ�ԭ����ʽ��native format�������ɵ���
��Ϣ��GDB ����ֱ�����������Ϣ������������Ҳ����ʹ�����������Ϣ

-ggdb
ʹ GCC Ϊ GDB ����ר�õĸ�Ϊ�ḻ�ĵ�����Ϣ�����ǣ���ʱ�Ͳ�������
���ĵ����������е����� (�� ddx)

-g �� -ggdb Ҳ�Ƿּ����

-g2
����Ĭ�ϵļ��𣬴�ʱ�����ĵ�����Ϣ������չ�ķ��ű��кš��ֲ�
���ⲿ������Ϣ��

-g3
��������2�е����е�����Ϣ���Լ�Դ�����ж���ĺ�

-g1
����1��-g1���������ֲ����������к��йصĵ�����Ϣ�����ֻ�ܹ����ڻ���
���ٺͶ�ջת��֮�á����ݸ���ָ���Ǽ��ӳ��������й����еĺ���������
ʷ����ջת������һ����ԭʼ��ʮ�����Ƹ�ʽ�������ִ�л����ķ���������
���Ǿ����õ��ĵ����ֶΡ�




�����/home/xiaowp/lib/Ŀ¼��������ʱ����Ҫ�Ŀ��ļ�libfoo.so��libfoo.a

Ϊ����GCC������ʱֻ�õ���̬���ӿ⣬����ʹ����������

gcc -o test test.c -L /home/xiaowp/lib -static -lfoo


-x ѡ�
-xѡ������Ԥ����ͷ�ļ�����


-E
���ѡ���ҵ�makefileû���ã���������ע��һ�����ѡ����ϣ���ɴ���ĺ�չ��������
����Ԥ���룬ʹ��-E����������GCC��Ԥ���������ֹͣ������̣�
gcc -E hello.c -o hello.i
���Ժ����ʱ���á�
����ֻ����Ԥ����,����������ļ�,����Ҫ�����ض���һ������ļ�����. 
���������÷�: 
����gcc -E hello.c > pianoapan.txt 
����gcc -E hello.c | more 
������������,һ��hello word ҲҪԤ�����800�еĴ��� 

-C 
������Ԥ�����ʱ��,��ɾ��ע����Ϣ,һ���-Eʹ��,��ʱ���������������ܷ����

-static 
������ѡ���ֹʹ�ö�̬�⣬����ʹ�ö�̬�⣬���ԣ���������Ķ�����һ�㶼�ܴ�Ҳ����Ҫʲô 
��̬���ӿ⣬�Ϳ�������. 
-share 
������ѡ�����ʹ�ö�̬�⣬���������ļ��Ƚ�С��������Ҫϵͳ�ɶ�̬��. 



-M 
���������ļ���������Ϣ������Ŀ���ļ�������������Դ�����������gcc -M hello.c 
������һ�£��ܼ򵥡� 
���� 
-MM 
������������Ǹ�һ������������������ϵͳͷ�ļ��������GCC3.0��ǰ����<>��""���ֵġ�
���� 
-MD 
������-M��ͬ��������������뵽.d���ļ����� 
���� 
-MMD 
������-MM��ͬ��������������뵽.d���ļ����� 


-pthread��-pthread��ͬ��-D_REENTRANT ����ʱ��ͬ��-lpthread
ע����߳̿���ʱ����Ҫ���-lrt����Ҫע��GCC 4.8�Ժ���������˳��ģ�
����Լ���Ϊ-lrt,-lpthread Ҫ�ŵ�����˳�������档

-rdynamic 
-rdynamicѡ������ӵ��Ƕ�̬���ӷ�����Ϣ�����ڶ�̬���ӹ��ܣ�����dlopen()ϵ�к�����
backtrace()ϵ�к���ʹ�ã����ܱ�strip������ǿ��strip�����³����޷�ִ�У�
-rdynamicȴ��һ������ѡ�����ָʾ�����������з��ţ���������ֻ�ǳ�����ʹ�õ����ⲿ���ţ�
����������̬���ţ����类static���εĺ���������ӵ���̬���ű���.dynsym���
�Ա���Щͨ��dlopen()��backtrace()����һϵ�к���ʹ��.dynsym���ڷ��ţ������ĺ���ʹ�á�
http://www.lenky.info/archives/2013/01/2190


32λ�棺���� -m32 ����������32λ�Ĵ��롣
64λ�棺���� -m64 ����������64λ�Ĵ��롣
