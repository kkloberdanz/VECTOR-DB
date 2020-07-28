.PHONY: release
release: server/target/release/vectrix

.PHONY: run
run: release
	cd server && cargo run --release

.PHONY: fmt
fmt:
	cd server && cargo fmt

.PHONY: debug
debug: server/target/debug/vectrix

server/target/release/vectrix: server/src/*.rs storage/libvectrix-storage.a
	cd server && cargo build --release

server/target/debug/vectrix: server/src/*.rs storage/libvectrix-storage.a
	cd server && cargo build

storage/libvectrix-storage.a: storage/*.c storage/*.h
	cd storage && $(MAKE) -j`nproc`

.PHONY: clean
clean:
	cd storage && $(MAKE) clean
	cd server && cargo clean
