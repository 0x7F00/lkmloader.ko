#include "linux/ptrace.h"
#include <linux/cred.h>
#include <linux/cpu.h>
#include <linux/memory.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <linux/task_work.h>
#include <asm-generic/errno-base.h>
#include <linux/slab.h>

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "lkmloader: " fmt
#endif

unsigned long (*kallsyms_lookup_name_fn)(const char*);
struct pt_regs *tmp_regs = NULL;

unsigned long __nocfi do_in_task_work_c() {
    unsigned long del_mod = kallsyms_lookup_name_fn("__arm64_sys_delete_module");
    pr_info("del_mod: 0x%lx\n", del_mod);
    if (!del_mod) {
        pr_err("could not del self\n");
        return 0;
    }
    struct pt_regs *regs = current_pt_regs();
    tmp_regs = kmemdup(regs, sizeof(struct pt_regs), GFP_KERNEL);
    if (!tmp_regs) {
        pr_err("failed to alloc tmp regs\n");
        return 0;
    }
    tmp_regs->regs[0] = tmp_regs->sp - sizeof("lkmloader");
    tmp_regs->regs[1] = 0;
    int ret = copy_to_user(tmp_regs->regs[0], "lkmloader", sizeof("lkmloader"));
    if (ret != 0) {
        pr_err("copy mod name failed: %d\n", ret);
        kfree(tmp_regs);
        return 0;
    }
    return del_mod;
}

void __naked do_in_task_work(struct callback_head *head) {
    asm(
        "stp x29, x30, [sp, #-0x10]!;\n"
        "bl do_in_task_work_c;\n"
        "mov x17, x0;\n"
        ".extern tmp_regs;\n"
        "ldr x0, =tmp_regs;\n"
        "ldr x0, [x0];\n"
        "ldp x29, x30, [sp], #0x10;\n"
        "br x17;\n"
    );
}

struct callback_head my_task_work = {
    .func = do_in_task_work
};

int __init __nocfi ko_init(void) {
	pr_info("lkmloader init\n");
    char buf[256];
    unsigned long addr, sz, found = 0, sprintf_addr;
    unsigned long aaddr = (unsigned long) &sprint_symbol;
    pr_info("sprint_symbol: 0x%lx [%pSb]\n", aaddr, (void *) aaddr);
    aaddr = (unsigned long) &ko_init;
    pr_info("ko_init: 0x%lx [%pSb]\n", aaddr, (void *) aaddr);

    asm("ldr %0, =sprint_symbol":"=r"(sprintf_addr));
    char *s;
    int ntry = 0;
    int maxtry = 1000000;
    addr = sprintf_addr - 4;
    bool is_found = false;
    // search backward
    for (;;) {
        sprintf(buf, "%pSb", (void*) addr);
        s = strchr(buf, '+');
        if (!s) {
            pr_err("no + found before addr-1\n");
            break;
        }
        *s = 0;
        if (strcmp(buf, "kallsyms_lookup_name") == 0) {
            pr_info("found kallsyms_lookup_name before %d\n", ntry);
            is_found = true;
        }
        s++;
        if (sscanf(s, "%lx", &sz) != 1) {
            pr_err("no off before sym\n");
            break;
        }
        if (is_found) {
            found = addr - sz;
            break;
        }
        addr -= sz + 4;
        ++ntry;
        --maxtry;
        if (maxtry == 0) {
            break;
        }
    }

    // search forward
    if (unlikely(!found && maxtry)) {
        pr_info("no kallsyms_lookup_name found before %d\n", ntry);
        ntry = 0;
        addr = sprintf_addr;
        // search forward
        for (;;) {
            sprintf(buf, "%pSb", (void*) addr);
            s = strchr(buf, '+');
            if (!s) {
                pr_err("no + found after %d syms\n", ntry);
                break;
            }
            *s = 0;
            // pr_info("after %d: %s\n", ntry, buf);
            if (strcmp(buf, "kallsyms_lookup_name") == 0) {
                pr_info("found kallsyms_lookup_name: after %d\n", ntry);
                found = addr;
                break;
            }
            if (strcmp(buf, "_end") == 0) {
                pr_err("hit _end after %d\n", ntry);
                break;
            }
            ++s;
            s = strchr(s, '/');
            if (!s) {
                pr_err("no / found after %d syms\n", ntry);
                break;
            }
            s++;
            if (sscanf(s, "%lx", &sz) != 1) {
                break;
            }
            if (sz == 0) {
                pr_warn("no size!\n");
                sz = 4;
            }
            addr += sz;
            ++ntry;
            --maxtry;
            if (maxtry == 0) {
                break;
            }
        }
    }
    if (!found) {
        pr_err("not found, remain try: %d\n", maxtry);
        return -ENOSYS;
    } else {
        pr_info("found kallsyms_lookup_name: 0x%lx!!!!!\n", found);
    }
    
    kallsyms_lookup_name_fn = (void*) found;
    unsigned long a = kallsyms_lookup_name_fn("kallsyms_lookup_name");
    pr_info("kallsyms_lookup_name find itself: 0x%lx\n", a);
    a = kallsyms_lookup_name_fn("_text");
    pr_info("kallsyms_lookup_name _text: 0x%lx\n", a);
    a = kallsyms_lookup_name_fn("_end");
    pr_info("kallsyms_lookup_name _end: 0x%lx\n", a);
    a = kallsyms_lookup_name_fn("task_work_add");
    pr_info("kallsyms_lookup_name task_work_add: 0x%lx\n", a);
    int (*task_work_add_fn)(struct task_struct *task, struct callback_head *twork,
			enum task_work_notify_mode mode) = a;

    int ret = task_work_add_fn(current, &my_task_work, TWA_RESUME);
    if (ret) {
        pr_err("task_work_add failed: %d\n", ret);
    }

    return 0;
}

void ko_exit(void) {
    if (!tmp_regs) {
        kfree(tmp_regs);
    }
	pr_info("lkmloader exit\n");
}

module_init(ko_init);
module_exit(ko_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("5ec1cff");
MODULE_DESCRIPTION("LKM Test");
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
