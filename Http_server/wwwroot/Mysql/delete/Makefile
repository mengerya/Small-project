.PHONY:all
all:delete index 

delete:delete.c
	gcc $^ -L/usr/lib64/mysql -lmysqlclient  -o $@

index:index.c
	gcc $^ -o $@

.PHONY:clean
clean:
	rm index delete

