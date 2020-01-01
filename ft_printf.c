/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obouykou <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/01 20:17:37 by obouykou          #+#    #+#             */
/*   Updated: 2020/01/01 20:20:26 by obouykou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

typedef struct s_fs
{
	char spec;
	int	width;
	int sign;
	int precision;
	int point;
	int len;
	int d;
	unsigned int u;
	long l;
	char *s;
	char *out;
	int ret;
}				t_fs;


void	ft_putchar(char c, int n, t_fs *fs)
{
	fs->ret += n;
	while (n--)
		write(1, &c, 1);
}


void	ft_putstr(char *s, t_fs *fs)
{
	if (fs->spec == 's' && s)
		while (fs->len--)
			ft_putchar(*s++, 1, fs);
	else if (s)
		while (*s)
			ft_putchar(*s++, 1, fs);
}


int		is_dig(char c)
{
	return (c >= '0' && c <= '9');
}


int		ft_atoi(char *s)
{
	int a = 0;
	while (is_dig(*s))
		a = (a * 10) + (*s++ - '0');
	return (a);
}


int		len(char *s)
{
	int l = 0;
	if (s)
		while (*s++)
			l++;
	return (l);
}


void	init_flag(t_fs *fs)
{
	fs->spec = 0;
	fs->len = 0;
	fs->sign = 0;
	fs->point = 0;
	fs->precision = 0;
	fs->width = 0;
	fs->out = "OMB";
}


void	gather_flag(t_fs *fs, char *s, va_list *ap)
{
	while (*s)
	{
		if (*s == 'd' || *s == 's' || *s == 'x')
		{
			fs->spec = *s;
			if (*s == 'd')
				fs->d = va_arg(*ap, int);
			else if (*s == 's')
				fs->s = va_arg(*ap, char *);
			else
				fs->u = va_arg(*ap, unsigned int);
			++s;
			break;
		}
		if (*s == '.')
		{
			fs->point = 1;
			fs->precision = ft_atoi(++s);
			while (is_dig(*s))
				s++;
		}
		if (is_dig(*s))
		{
			fs->width = ft_atoi(s);
			while (is_dig(*s))
				s++;
		}
	}
	fs->out = s;
}


int		nbrlen(int n)
{
	int l;

	l = 1;
	while (n / 10)
	{
		n /= 10;
		l++;
	}
	return (l);
}


void	correct_dec(t_fs *fs)
{
	fs->len = nbrlen(fs->d);
	if (fs->d < 0)
		fs->sign = 1;
	fs->l = fs->sign ? -(long)fs->d : (long)fs->d;
	if (!fs->l && fs->point && fs->precision == 0)
		fs->len = 0;
	fs->precision -= (fs->precision > fs->len) ? fs->len : fs->precision;
	fs->width -= (fs->width > fs->precision + fs->len + fs->sign) ? fs->precision + fs->len + fs->sign : fs->width;
}


void	ft_putnbr(long l, t_fs *fs)
{
	if (l > 9)
		ft_putnbr(l / 10, fs);
	ft_putchar((l % 10) + '0', 1, fs);
}


void	print_dec(t_fs *fs)
{
	fs->width ? ft_putchar(' ', fs->width, fs) : 0;
	fs->sign ? ft_putchar('-', 1, fs) : 0;
	fs->precision ? ft_putchar('0', fs->precision, fs) : 0;
	if (fs->len)
		ft_putnbr(fs->l, fs);
}


char	hex_dig(int	n)
{
	if (n < 10)
		return (n + '0');
	else
		return (n + 87);
}


char * hex_conv(unsigned int u, int step)
{
	static char *hex;
	static int l;

	if (!step)
	{
		hex = (char *)malloc(17);
		int i = 0;
		while (i < 17)
			hex[i++] = 0;
		l = 0;
	}
	if (u / 16)
		hex_conv(u / 16, 1);
	hex[l++] = hex_dig(u % 16);
	return (hex);
}

void	correct_hex(t_fs *fs)
{
	fs->s = hex_conv(fs->u, 0);
	fs->len = len(fs->s);
	if (!fs->u && fs->point && fs->precision == 0)
		fs->len = 0;
	fs->precision -= (fs->precision > fs->len) ? fs->len : fs->precision;
	fs->width -= (fs->width > fs->precision + fs->len) ? fs->precision + fs->len : fs->width;
}


void	print_hex(t_fs *fs)
{
	fs->width ? ft_putchar(' ', fs->width, fs) : 0;
	fs->precision ? ft_putchar('0', fs->precision, fs) : 0;
	if (fs->len)
		ft_putstr(fs->s, fs);
//	free(fs->s);
}


void	correct_str(t_fs *fs)
{
	fs->len = fs->s != NULL ? len(fs->s) : 6;
	fs->len = (fs->point && fs->len > fs->precision) ? fs->precision : fs->len;
	fs->width -= (fs->width > fs->len) ? fs->len : fs->width;
}


void	print_str(t_fs *fs)
{
	fs->width ? ft_putchar(' ', fs->width, fs) : 0;
	if (fs->s != NULL)
		ft_putstr(fs->s, fs);
	else if (fs->s == NULL)
		ft_putstr("(null)", fs);
}


void 	global_print(t_fs *fs)
{
	if (fs->spec == 'd')
	{
		correct_dec(fs);
		print_dec(fs);
	}
	else if (fs->spec == 'x')
	{
		correct_hex(fs);
		print_hex(fs);
	}
	else if (fs->spec == 's')
	{
		correct_str(fs);
		print_str(fs);
	}
}


int		ft_printf(const char *s, ...)
{
	va_list ap;
	t_fs fs;

	fs.ret = 0;
	va_start(ap, s);
	while (*s)
	{
		while (*s && *s != '%')
			ft_putchar(*s++, 1, &fs);
		if (*s && *s == '%')
		{
			init_flag(&fs);
			gather_flag(&fs, (char *)++s, &ap);
			global_print(&fs);
			s = fs.out;
		}
	}
	return (fs.ret);
}

#include <stdio.h>

#define exp "|hello %.x %s %s|", 1551255, "hello", NULL

int main(void)
{
	printf("\t|R0=%d|\n", printf(exp));
	printf("\t|RF=%d|\n", ft_printf(exp));
}
