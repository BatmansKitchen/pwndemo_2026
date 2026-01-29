FROM ubuntu
RUN apt-get update && apt-get install -y \
    g++ make gdb curl git socat procps runit openssh-server \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /app

COPY basic1 basic1.c /app/

RUN useradd -m bunny && echo "bunny:bunny" | chpasswd
RUN echo "exec /usr/local/bin/gdb_wrapper.sh" >> /home/bunny/.profile
RUN echo "set debuginfod enabled off" >> /home/bunny/.gdbinit

RUN mkdir /var/run/sshd
RUN echo 'PermitRootLogin no' >> /etc/ssh/sshd_config
RUN echo 'PasswordAuthentication yes' >> /etc/ssh/sshd_config

RUN chmod 555 /app/* && chown root:root /app/*

COPY gdb_wrapper.sh run_ssh.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/gdb_wrapper.sh /usr/local/bin/run_ssh.sh

CMD ["/usr/local/bin/run_ssh.sh"]