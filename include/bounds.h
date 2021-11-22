/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bounds.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wkorande <wkorande@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/01 12:53:02 by wkorande          #+#    #+#             */
/*   Updated: 2020/04/13 17:19:06 by wkorande         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace wgl
{

struct Bounds
{
	float x_min;
	float x_max;
	float y_min;
	float y_max;
	float z_min;
	float z_max;
};

}