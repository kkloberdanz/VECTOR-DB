.PHONY: release
release: server/target/release/vector-db-server

.PHONY: debug
debug: server/target/debug/vector-db-server

server/target/release/vector-db-server: storage/libvectordb-storage.a
	cd server && cargo build --release

server/target/debug/vector-db-server: storage/libvectordb-storage.a
	cd server && cargo build

storage/libvectordb-storage.a: storage/file.c storage/ktmath.c storage/*.h
	cd storage && $(MAKE) -j`nproc`

.PHONY: clean
clean:
	cd storage && $(MAKE) clean
