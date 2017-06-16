<?php //netteCache[01]000390a:2:{s:4:"time";s:21:"0.25115800 1355063151";s:9:"callbacks";a:2:{i:0;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:9:"checkFile";}i:1;s:68:"/home/users/xd/xdlapa01/WWW/IIS/app/templates/Homepage/default.latte";i:2;i:1355059728;}i:1;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:10:"checkConst";}i:1;s:25:"Nette\Framework::REVISION";i:2;s:30:"6a33aa6 released on 2012-10-01";}}}?><?php

// source file: /home/users/xd/xdlapa01/WWW/IIS/app/templates/Homepage/default.latte

?><?php
// prolog Nette\Latte\Macros\CoreMacros
list($_l, $_g) = Nette\Latte\Macros\CoreMacros::initRuntime($template, '3kuwq2p3bl')
;
// prolog Nette\Latte\Macros\UIMacros
//
// block content
//
if (!function_exists($_l->blocks['content'][] = '_lb7214ae05e3_content')) { function _lb7214ae05e3_content($_l, $_args) { extract($_args)
;call_user_func(reset($_l->blocks['title']), $_l, get_defined_vars())  ?>

<div id="container">
    <div id="sidebar">
        <div class="menu">
            <ul>
<?php $iterations = 0; foreach ($menuItems as $item => $link): ?>                <li><a href="<?php echo htmlSpecialChars($_control->link($link)) ?>
"><?php echo Nette\Templating\Helpers::escapeHtml($item, ENT_NOQUOTES) ?></a></li>
<?php $iterations++; endforeach ?>            </ul>
        </div>
    </div><?php
}}

//
// block title
//
if (!function_exists($_l->blocks['title'][] = '_lb542dca3ebf_title')) { function _lb542dca3ebf_title($_l, $_args) { extract($_args)
?><h1>Vítejte v IS lékárny</h1>
<?php
}}

//
// end of blocks
//

// template extending and snippets support

$_l->extends = empty($template->_extended) && isset($_control) && $_control instanceof Nette\Application\UI\Presenter ? $_control->findLayoutTemplateFile() : NULL; $template->_extended = $_extended = TRUE;


if ($_l->extends) {
	ob_start();

} elseif (!empty($_control->snippetMode)) {
	return Nette\Latte\Macros\UIMacros::renderSnippets($_control, $_l, get_defined_vars());
}

//
// main template
//
?>

<?php if ($_l->extends) { ob_end_clean(); return Nette\Latte\Macros\CoreMacros::includeTemplate($_l->extends, get_defined_vars(), $template)->render(); }
call_user_func(reset($_l->blocks['content']), $_l, get_defined_vars()) ; 