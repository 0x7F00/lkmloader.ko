FROM ghcr.io/ylarod/ddk:android16-6.12
RUN apt update && apt install gnupg2 sudo ssh -y && passwd -d ubuntu
RUN [ -d /opt/ddk/src/android16-6.12 ] && sudo sed -i \
  -e 's|^$(obj)/%.s: $(obj)/%.c FORCE$|$(obj)/%.s: $(src)/%.c FORCE|' \
  -e 's|^$(obj)/%.i: $(obj)/%.c FORCE$|$(obj)/%.i: $(src)/%.c FORCE|' \
  -e 's|^$(obj)/%.symtypes : $(obj)/%.c FORCE$|$(obj)/%.symtypes : $(src)/%.c FORCE|' \
  -e 's|^$(obj)/%.ll: $(obj)/%.c FORCE$|$(obj)/%.ll: $(src)/%.c FORCE|' \
  -e 's|^$(obj)/%.o: $(obj)/%.c $(recordmcount_source) FORCE$|$(obj)/%.o: $(src)/%.c $(recordmcount_source) FORCE|' \
  -e 's|^$(obj)/%.lst: $(obj)/%.c FORCE$|$(obj)/%.lst: $(src)/%.c FORCE|' \
  -e 's|^$(obj)/%.symtypes : $(obj)/%.S FORCE$|$(obj)/%.symtypes : $(src)/%.S FORCE|' \
  -e 's|^$(obj)/%.s: $(obj)/%.S FORCE$|$(obj)/%.s: $(src)/%.S FORCE|' \
  -e 's|^$(obj)/%.o: $(obj)/%.S FORCE$|$(obj)/%.o: $(src)/%.S FORCE|' \
  /opt/ddk/src/android16-6.12/scripts/Makefile.build
ENV GIT_EDITOR 'code --wait'
