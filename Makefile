.PHONY: release
release: server/target/release/vector-db-server

.PHONY: run
run: release
	cd server && cargo run --release

.PHONY: fmt
fmt:
	cd server && cargo fmt

.PHONY: debug
debug: server/target/debug/vector-db-server

server/target/release/vector-db-server: server/src/*.rs storage/libvectordb-storage.a
	cd server && cargo build --release

server/target/debug/vector-db-server: server/src/*.rs storage/libvectordb-storage.a
	cd server && cargo build

storage/libvectordb-storage.a: storage/*.c storage/*.h
	cd storage && $(MAKE) -j`nproc`

.PHONY: clean
clean:
	cd storage && $(MAKE) clean
	cd server && cargo clean
