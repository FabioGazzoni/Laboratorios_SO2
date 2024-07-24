EXEC_PATH=$(shell pwd)/server/service

build:
	cd server && go build && cd ..

copy-config:
	sudo cp config_files/default /etc/nginx/sites-available/
	sudo sed -i 's|ExecStart=.*|ExecStart=$(EXEC_PATH)|' config_files/serviceSOII.service
	sudo cp config_files/serviceSOII.service /etc/systemd/system/

update-hosts:
	echo "127.0.0.1	dashboard.com" | sudo tee -a /etc/hosts
	echo "127.0.0.1	sensors.com" | sudo tee -a /etc/hosts

start-service:
	sudo systemctl start nginx  
	sudo systemctl daemon-reload
	sudo systemctl start serviceSOII.service
	sudo systemctl enable serviceSOII.service

install:
	make build
	make copy-config
	make update-hosts
	make start-service

stop:
	sudo systemctl stop serviceSOII.service
	sudo systemctl disable serviceSOII.service

clean:
	make stop
	rm -rf server/service
	sudo rm /etc/nginx/sites-available/default
	sudo systemctl stop nginx
	sudo systemctl disable nginx
	sudo rm /etc/systemd/system/serviceSOII.service
	sudo rm -rf server/service
	sudo sed -i '/dashboard.com/d' /etc/hosts
	sudo sed -i '/sensors.com/d' /etc/hosts