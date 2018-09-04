.PHONY:all
all:find index 

find:find.c
	gcc $^ -L/usr/lib64/mysql -lmysqlclient  -o $@

index:index.c
	gcc $^ -o $@

.PHONY:clean
clean:
	rm index find
