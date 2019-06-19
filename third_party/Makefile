
RM        ?= rm -f
MKDIR     ?= mkdir -p
RMDIR     ?= rm -rf
MAKE      ?= make
COPY      ?= cp -r
CD        ?= cd

DEPINCUDE_DIR=./include
INC_LUALIB= $(DEPINCUDE_DIR)/lualib-5.1.5
INC_PROTOBUF=$(DEPINCUDE_DIR)/protobuf-2.6.1
INC_SQLITE=$(DEPINCUDE_DIR)/sqlite-3.8.8.1
INC_MYSQL=$(DEPINCUDE_DIR)/mysql-linux-5.6.22
INC_RAPIDXML=$(DEPINCUDE_DIR)/rapidxml-1.13


OS_BITS := $(shell getconf LONG_BIT)

ifeq ($(OS_BITS),32)
DEPENDLIB_DIR= ../lib/linux/x86
endif
ifeq ($(OS_BITS),64)
DEPENDLIB_DIR= ../lib/linux/x64
endif

DEPENDBIN_DIR=../bin

INSTALL_DIR=/usr/local


.PHONY : all clean install

#��ɱ��룬���Ұ��ļ��ŵ���Ӧ��libĿ¼�£�
#google protobuf��Ҫִ��configure
#Lualib��makefineû�и�install���Լ����ѡ������˼Ҵ����ˡ�
all:
	$(MKDIR) $(DEPENDBIN_DIR)
	$(MKDIR) $(DEPENDLIB_DIR)
	@echo "====================================================================";
	$(MAKE) -C $(INC_LUALIB) linux
	$(COPY) $(INC_LUALIB)/liblua.a $(DEPENDLIB_DIR)
	$(COPY) $(INC_LUALIB)/luac $(DEPENDBIN_DIR)
	$(COPY) $(INC_LUALIB)/lua  $(DEPENDBIN_DIR)
	@echo "====================================================================";
	@depend_lib=`pwd`; \
	$(CD) $$depend_lib/$(INC_PROTOBUF); \
	./configure; \
	$(CD) $$depend_lib 
	$(MAKE) -C $(INC_PROTOBUF)
	$(COPY) $(INC_PROTOBUF)/src/.libs/libprotobuf.a $(DEPENDLIB_DIR)
	$(COPY) $(INC_PROTOBUF)/src/.libs/libprotobuf-lite.a $(DEPENDLIB_DIR)
	$(COPY) $(INC_PROTOBUF)/src/.libs/protoc $(DEPENDBIN_DIR)
	@echo "====================================================================";
	$(MAKE) -C $(INC_SQLITE)
	$(COPY) $(INC_SQLITE)/libsqlite.a $(DEPENDLIB_DIR)
	
#�������Ҫ���ļ���װ��/usr/localĿ¼��	
#�����ϣ������/usr/local���ļ��ɸɾ������������make;make clean�ˣ���make install
install:
	@echo "===================================================================="
	$(COPY) $(DEPENDBIN_DIR)/* $(INSTALL_DIR)/bin
	@echo "===================================================================="
	$(COPY) $(DEPINCUDE_DIR)/*  $(INSTALL_DIR)/include
	@echo "===================================================================="
	$(COPY) $(DEPENDLIB_DIR)/*  $(INSTALL_DIR)/lib

clean:
	@echo "===================================================================="
	$(MAKE) -C $(INC_LUALIB) clean
	@echo "===================================================================="
	$(MAKE) -C $(INC_PROTOBUF) clean
	@echo "===================================================================="
	$(MAKE) -C $(INC_SQLITE) clean