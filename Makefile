APP_NAME         := URLShortener
DOCKER_DIR       := ops/docker
LOCAL_BUILD_DIR  := build
DOCKER_PORT      := 8080
VCPKG_TOOLCHAIN  := $(HOME)/vcpkg/scripts/buildsystems/vcpkg.cmake
SERVICES         := url_shortener
NAMESPACE        := shortener
PREFIX           := dev
PREFIXNAME       := $(PREFIX)-$(NAMESPACE)
GREEN=\033[0;32m
NC=\033[0m # No Color

.PHONY: all local clean test test-run image deploy deploy-update
all: image

local:
	@mkdir -p $(LOCAL_BUILD_DIR)
	cmake \
	  -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DBUILD_TESTS=OFF \
	  -S . -B $(LOCAL_BUILD_DIR)

	cmake --build $(LOCAL_BUILD_DIR) --parallel

clean:
	cmake --build $(LOCAL_BUILD_DIR) --target clean
	rm -rf $(LOCAL_BUILD_DIR)

test:
	@mkdir -p $(LOCAL_BUILD_DIR)
	cmake \
	  -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DBUILD_TESTS=ON \
	  -S . \
	  -B $(LOCAL_BUILD_DIR)
	@cmake --build $(LOCAL_BUILD_DIR) --parallel

test-run:
	@cd $(LOCAL_BUILD_DIR) && ctest --output-on-failure

image-%:
	@$(MAKE) image SERVICES="$*"

base:
	docker build -f $(DOCKER_DIR)/dockerfile.deps -t $(PREFIX)-$(NAMESPACE)/urlshortener-deps:latest .

image:
	@echo "Starting to build services..."
	@for service in $(SERVICES); do \
 		echo "Building $$service..."; \
		docker build -f $(DOCKER_DIR)/dockerfile.$$service --no-cache \
		           -t $(PREFIXNAME)-$$service:latest .; \
		docker tag $(PREFIXNAME)-$$service:latest \
		           localhost:32000/$(PREFIXNAME)-$$service:latest; \
		docker push localhost:32000/$(PREFIXNAME)-$$service:latest; \
		echo  "$(GREEN)$$service built!$(NC)"; \
	done
	@echo "$(GREEN)Build process complete!$(NC)"

deploy:
	microk8s kubectl create namespace $(NAMESPACE) || true
	microk8s kubectl apply -k ops/k8s/dev -n $(NAMESPACE)
	@echo "$(GREEN)Deployed successfully!$(NC)"

deploy-update:
	microk8s kubectl delete namespace $(NAMESPACE)
	@$(MAKE) deploy

cleanup:
	docker image prune -f
